/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Implementation of PipeLine.hpp.
 *
 * To do:
 *		Move the render cache schedule code into the 'ExecuteScheduleForCaches' function.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/PipeLine.cpp                                             $
 * 
 * 366   98/10/09 0:32 Speter
 * More lenient polygon estimate to fix assert.
 * 
 * 365   9/29/98 9:33p Mlange
 * Fixed bug in spatial partition intersect code when terrain height relative was enabled.
 * 
 * 364   9/26/98 8:08p Pkeet
 * Removed the 'static' keyword from stats that need to be externed.
 * 
 * 363   9/25/98 1:50a Pkeet
 * Moved assert.
 * 
 * 362   98.09.24 8:34p Mmouni
 * Unlit now turns off the LIGHT_SHADE flag.
 * 
 * 361   98.09.24 1:41a Mmouni
 * Vertices are now commited per polygon instead of per shape, greatly reducing the amount of
 * memory we commit.
 * 
 **********************************************************************************************/

//
// Defines and pragmas.
//

#define bVIEWER_INFINITY		(0)
#define bDISTANCE_CULL_TERRAIN	(0)

#include "Common.hpp"
#include "Lib/W95/WinInclude.hpp"
#include "D3DTypes.h"
#include "Lib/GeomDBase/PartitionPriv.hpp"
#include "PipeLine.hpp"
#include "Overlay.hpp"

#include "RenderDefs.hpp"
#include "Fog.hpp"
#include "Light.hpp"
#include "Camera.hpp"
#include "ScreenRender.hpp"
#include "RenderCacheInterface.hpp"
#include "PipeLineHelp.hpp"
#include "PipeLineHeap.hpp"
#include "DepthSort.hpp"
#include "RenderCache.hpp"
#include "Primitives/FastBump.hpp"
#include "Lib/Sys/Profile.hpp"
#include "Lib/Math/FastTrig.hpp"
#include "Lib/EntityDBase/Instance.hpp"
#include "Lib/EntityDBase/Query/QTerrain.hpp"
#include "Lib/Math/FloatDef.hpp"
#include "Lib/Renderer/Occlude.hpp"
#include "Lib/Sys/Scheduler.hpp"
#include "Lib/GeomDBase/Shape.hpp"
#include "Lib/GeomDBase/MeshIterator.hpp"
#include "Lib/GeomDBase/SkeletonIterator.hpp"
#include "Lib/GeomDBase/WaveletQuadTree.hpp"
#include "Lib/Sys/ConIO.hpp"
#include "Lib/Sys/W95/Render.hpp"
#include "ShapePresence.hpp"
#include "Lib/Renderer/ScreenRenderAuxD3DBatch.hpp"
#include "Game/Ai/AIMain.hpp"
#include "Particles.hpp"
#include "Lib/View/Direct3DRenderState.hpp"

#if VER_MULTI_RES_WATER
#include "Lib/GeomDBase/WaterQuadTree.hpp"
#endif

#include "Lib/Renderer/ScreenRenderAuxD3D.hpp"
#include "Lib/W95/Direct3D.hpp"
#include "Lib/Std/LocalArray.hpp"

#define bINIDIVIDUAL_POLY_STATS (0)
#define bCLIPPING_CHECK			(VER_DEBUG)
#define bZBUFFER_BATCH_ONLY     (0)


//
// Global variables.
//

// Sub-stats for render shape.
static CProfileStat psUnseenCaches	("CacheUnseen", &proProfile.psRender);
static CProfileStat psIntersect		("RenderPart", &proProfile.psRender),
						psIntersectSetup	("Setup", &psIntersect, Set(epfHIDDEN)),
						psIntersectAdjust	("Adjust Ht", &psIntersect/*, Set(epfHIDDEN)*/),
						psIntersectCam		("Cam", &psIntersect/*, Set(epfHIDDEN)*/),
						psIntersectOcclude	("Occlude", &psIntersect/*, Set(epfHIDDEN)*/),
						psIntersectDetail	("Detail", &psIntersect, Set(epfHIDDEN)),
						psIntersectPreRender("PreRender", &psIntersect, Set(epfHIDDEN));
	   CProfileStat		psCacheSched		("Cache Sched", &psIntersect);
static CProfileStat psProject		("Project", &proProfile.psRender);
static CProfileStat psUploadTextures("D3D Uploads", &proProfile.psRender);
static CProfileStat psBatchOther("Batch Other", &proProfile.psRender);
static CProfileStat psBatchCaches("Batch Caches", &proProfile.psRender);
static CProfileStat psFeature		("Feature+Mip", &proProfile.psRender);

CProfileStat psExecuteCaches("Execute Caches", &proProfile.psRender);
CProfileStat psExecuteTerrain("Execute Terrain", &proProfile.psRender);


#if (bINIDIVIDUAL_POLY_STATS)

static CProfileStat psShapeSetup	("Setup", &proProfile.psRenderShape);
static CProfileStat psCull			("Cull", &proProfile.psRenderShape);
static CProfileStat psTransform		("Transform", &proProfile.psRenderShape);
static CProfileStat psView			("View", &proProfile.psRenderShape);
static CProfileStat psOcclude		("Occlude", &proProfile.psRenderShape);
static CProfileStat psPolygons		("Polygons", &proProfile.psRenderShape);
static CProfileStat psVertices		("Vertices", &proProfile.psRenderShape);
static CProfileStat psAllVertices	/*("AllVertices", &proProfile.psRenderShape)*/;
static CProfileStat psFaceLight		("FaceLight", &proProfile.psRenderShape);
static CProfileStat psLight			("Light", &proProfile.psRenderShape);
static CProfileStat psClip			("Clip", &proProfile.psRenderShape);
static CProfileStat psFog			("Fog", &proProfile.psRenderShape);

#define I_STAT_ADD(stat, a, b)	stat.Add((a), (b))

#else

CProfileStat psRenderShapeReg	("Shape-reg", &proProfile.psRenderShape);
CProfileStat psRenderShapeBio	("Shape-bio", &proProfile.psRenderShape);
CProfileStat psRenderShapeTrr	("Shape-trr", &proProfile.psRenderShape);
CProfileStat psRenderShapeImc	("Shape-imc", &proProfile.psRenderShape);

#define I_STAT_ADD(stat, a, b)

#endif

CProfileStat psTerrain	/*("Terrain", &proProfile.psRender)*/;

extern void ValidatePolylist(CPArray<CRenderPolygon*>& parpoly);
extern void DumpPolylist(CPArray<CRenderPolygon*>& parpoly);

// Paranoid check of depth sort order in debug mode.
#define bVERIFY_DEPTHSORT_ORDER (0)

// Max number of vertices for using simple occlusion.
#define iMAX_SIMPLE_OCCLUDE (8)

// Scratch memory for occlusion.
CVector3<> av3Occlude[iMAX_SIMPLE_OCCLUDE];
CPArray< CVector3<> > pav3CamOccludeVertices(iMAX_SIMPLE_OCCLUDE, av3Occlude);

// Tiling test.
const CSet<ERenderFeature> erfsetTiling = Set(erfTILE_UV) + erfTILE_U + erfTILE_V;


//**********************************************************************************************
//
template<class S> class CRenderShape
//
// A class for rendering various shape types, with inline access to shape iterator.
// This is a class rather than a function so we can explicitly specify template types.
// Otherwise, retarded VC4.2 fails in inferring the template type from the function
// parameters.
// 
//**************************************
{
public:

	//******************************************************************************************
	//
	CRenderShape
	(
		const S&				shape,				// Shape to render.
		const CInstance*		pins,				// Instance owning the shape.
		CRenderContext&			renc,				// All the general info needed for rendering.
		CShapePresence&         rsp,				// This shape's world presence.
		const CTransform3<>&    tf3_shape_camera,	// Shape to camera transform.
		const CPArray<COcclude*>&papoc,				// Array of occluding objects.
		ESideOf					esf_view			// This shape's relation to the view volume.
	)
	//
	// Renders the shape to an unprojected polygon list.
	//
	//**********************************
	{
		CCycleTimer ctmr, ctmr_total;

		// Useful flags.
		bool b_use_hardware = d3dDriver.bUseFullTexturing() && renc.Renderer.bTargetScreen() && pins->pdGetData().bHardwareAble;
		//bool b_use_hardware = false;

		//
		// If the shape is uncacheable, and the destination raster is a render cache, do
		// nothing.
		//
		if (!shape.bTerrain() && !shape.bIsCacheable() && renc.Renderer.pSettings->bTargetCache)
			return;

		#if VER_DEBUG
			shape.Validate();
		#endif

		//
		// Setup.
		// Construct a bunch of transforms for converting between object, world, and camera spaces.
		//

		// Copy var locally to avoid derefs.
		const CRenderer::SSettings* p_settings = renc.Renderer.pSettings;

		// Set the lighting context, so that lights will transform into object space.
		if (p_settings->seterfState[erfLIGHT])
		{
			renc.rLightList.SetViewingContext(rsp.pr3GetWorldShape(), rsp.pr3GetCamShape());
		}

		//
		// Create the polygon iterator.
		//
		S::CPolyIterator pi(shape, pins, &renc);
	
		//
		// Pre-commit memory sufficient for the polygons of this shape.
		// Check bCommit return value in the unlikely case we're out of heap.
		//
		uint u_poly_count	= pi.iNumPolygons() * 2 + 32;

		if (!renc.rplhHeap.darpolyPolygons. bCommit(u_poly_count) ||
			!renc.rplhHeap.darppolyPolygons.bCommit(u_poly_count))
			return;

#if VER_TEST
		uint u_poly_limit	= renc.rplhHeap.darpolyPolygons.uLen + u_poly_count,
			 u_polyp_limit	= renc.rplhHeap.darppolyPolygons.uLen + u_poly_count;
#endif

		//
		// Create an array mapping the shape vertices to render vertices.
		// If vertices are not shared, then no mapping is tracked.
		//
		int i_num_vertices = shape.bSharedVertices() ? pi.iNumShapeVertices() : 0;
		CLArray(SRenderVertex*, parv_mapping, i_num_vertices);
		parv_mapping.Fill(0);

		I_STAT_ADD(psShapeSetup, ctmr(), 1);

		// Create an array of transformed points/outcodes for each unique shape point.
		int i_num_shape_points = pi.iNumPoints();
		CLArray(SClipPoint, paclpt_points, i_num_shape_points);

		// Transform [and outcode] all unique points in the shape at once.
		pi.TransformPoints(tf3_shape_camera, renc.Camera, paclpt_points, (esf_view & esfOUTSIDE) | bCLIPPING_CHECK);

		I_STAT_ADD(psTransform, ctmr(), i_num_shape_points);

		CVector3<> v3_cam_shape = rsp.pr3GetCamShape().v3Pos;

		//
		// Iterate through the shape's polygons.
		//
		while (pi.bNext())
		{
			//
			// Skip polygons that exist for occlusion only.
			//
			if (pi.bOcclude())
				continue;

			if (pi.bBackface(v3_cam_shape))
				continue;

			I_STAT_ADD(psCull, ctmr(), 1);

			//
			// Process the polygon points.
			//
			CSet<EOutCode> seteoc_poly;

			if ((esf_view & esfOUTSIDE) || bCLIPPING_CHECK)
			{
				CSet<EOutCode> seteoc_poly_all = -CSet<EOutCode>();

				for (int i_v = 0; i_v < pi.iNumVertices(); i_v++)
				{
					// Get point index.
					int i_point_index = pi.iShapePoint(i_v);

					// Combine outcodes of all points.
					seteoc_poly     += paclpt_points[i_point_index].seteocOut;
					seteoc_poly_all &= paclpt_points[i_point_index].seteocOut;
				}
				
				#if (bCLIPPING_CHECK)
					if (seteoc_poly - eocFAR)
					{
						// A point is outside the view volume, so let's make sure we
						// weren't told to disable clipping. We ignore the far clipping
						// plane for now, since if it's violated it's not critical, and 
						// the render cache may have problems staying within in.
						AlwaysAssert(esf_view & esfOUTSIDE);
					}
				#endif

				//
				// Perform a quick intersection test with the camera volume.
				// If seteoc_poly == 0, the polygon is entirely inside the view volume, and
				// no clipping will be done later.
				// Otherwise, the polygon may intersect one or more planes, and must clip it later.
				//
				I_STAT_ADD(psView, ctmr(), pi.iNumVertices());

				// See whether we can reject the polygon.
				if (seteoc_poly_all)
				{
					// All points are outside at least one plane.
					continue;
				}
			}

			//
			// Attempt to occlude the polygon.
			//
			if (COcclude::bUsePolygonOcclusion && papoc.uLen)
			{
				#if bINIDIVIDUAL_POLY_STATS
					CTimeBlock tmb(&psOcclude);
				#endif

#if (1)
				if (pi.iNumVertices() < iMAX_SIMPLE_OCCLUDE)
				{
					pav3CamOccludeVertices.uLen = pi.iNumVertices();

					for (int i_v = 0; i_v < pi.iNumVertices(); i_v++)
						pav3CamOccludeVertices[i_v] = paclpt_points[pi.iShapePoint(i_v)].v3Point;

					// Skip this polygon if it is occluded.
					if (bOccludePolygon(papoc, pav3CamOccludeVertices))
						continue;
				}
				else
				{
					// Array of transformed vertices.
					CLArray(CVector3<>, pav3_cam_vertices, pi.iNumVertices());

					for (int i_v = 0; i_v < pi.iNumVertices(); i_v++)
						pav3_cam_vertices[i_v] = paclpt_points[pi.iShapePoint(i_v)].v3Point;

					// Skip this polygon if it is occluded.
					if (bOccludePolygon(papoc, pav3_cam_vertices))
						continue;
				}
#else
				bool b_occluded = true;

				// Iterate through the occlusion objects looking for occlusion or intersection.
				for (uint u = 0; u < papoc.uLen; ++u)
				{
					//
					// If the points are entirely inside the occluding object's bounding volume,
					// the polygon is occluded.
					//
					for (int i_v = 0; i_v < pi.iNumVertices(); i_v++)
					{
						if (!papoc[u]->bInsideNormPlanes(paclpt_points[pi.iShapePoint(i_v)].v3Point))
						{
							// Not occluded
							b_occluded = false;
							break;
						}
					}

					if (b_occluded)
						break;
				}

				if (b_occluded)
					continue;
#endif
			}

			//
			// Create a new render polygon on the array.
			//

			// Use ptFastAlloc() rather than new, to bypass the default constructor.
			// Call InitFast() instead.
			CRenderPolygon& rpoly = *renc.rplhHeap.darpolyPolygons.ptFastAlloc();
			rpoly.InitFast();
			rpoly.bPrerasterized = false;

#if (VER_DEBUG)
			rpoly.pshOwningShape = (CShape *)&shape;
#endif

			// Mark the polygon for using Direct3D.
			rpoly.bFullHardware  = b_use_hardware;

			rpoly.ptexTexture = pi.ptexTexture();
			Assert(rpoly.ptexTexture);

		#if VER_DEBUG
			rpoly.ptexTexture->Validate();
		#endif

			// Find the intersection of the global and polygon render features.
			{
				CSet<ERenderFeature> erf = rpoly.ptexTexture->seterfFeatures;
				rpoly.seterfFace = erf & p_settings->seterfState;

				// Set tiling features.
				if (b_use_hardware)
				{
					rpoly.eamAddressMode = eamTileNone;
					if (erf & erfsetTiling)
					{
						if (erf[erfTILE_UV])
						{
							rpoly.eamAddressMode = eamTileUV;
						}
						else
						{
							if (erf[erfTILE_U])
								rpoly.eamAddressMode = eamTileU;
							if (erf[erfTILE_V])
								rpoly.eamAddressMode = eamTileV;
						}
					}
				}
			}

			// Add the clip feature flag if required.
			if (shape.bTerrain() && p_settings->bTargetCache)
			{
				rpoly.seterfFace = Set(erfDRAW_CLIP);
			}

			I_STAT_ADD(psPolygons, ctmr(), 1);

			//
			// Allocate the polygon vertices.
			//

			// Vertex pointers must be allocated on the vertex pointer heap.
			rpoly.paprvPolyVertices = renc.rplhHeap.daprvVertices.paAlloc(pi.iNumVertices());

			if (shape.bSharedVertices())
			{
				// Commit as many vertices as we will possibly neeed.
				renc.rplhHeap.darvVertices.bCommit(pi.iNumVertices());

				// Shared vertices.
				for (int i_v = 0; i_v < pi.iNumVertices(); i_v++)
				{
					// See if this vertex has been mapped to a RenderPolygon yet.
					int i_shape_vertex = pi.iShapeVertex(i_v);
					if (parv_mapping[i_shape_vertex])
					{
						// Vertex has already been evaluated.
						// Place a pointer to it in the RenderPolygon.
						rpoly.paprvPolyVertices[i_v] = parv_mapping[i_shape_vertex];
						continue;
					}

					//
					// This vertex has not yet been processed.
					// Create a RenderVertex to contain rendering info.
					//
					SRenderVertex* prv = renc.rplhHeap.darvVertices.ptFastAlloc();

					// Store the new mapping.
					rpoly.paprvPolyVertices[i_v] = parv_mapping[i_shape_vertex] = prv;

					// Store the transformed point.
					Assert(!paclpt_points[pi.iShapePoint(i_v)].seteocOut[eocUNINIT]);
					prv->v3Cam = paclpt_points[pi.iShapePoint(i_v)].v3Point;

					// Copy the texture coord.
 					prv->tcTex = pi.tcTexCoord(i_v);

					// Set lighting value to negative so we know to light it.
					prv->cvIntensity = -1.0;

					I_STAT_ADD(psVertices, 0, 1);
				}
			}
			else
			{
				// Unique vertices.  Allocate all at once.
				SRenderVertex* aprv = renc.rplhHeap.darvVertices.paAlloc(pi.iNumVertices());

				// Init them.
				for (int i_v = 0; i_v < pi.iNumVertices(); i_v++)
				{
					SRenderVertex* prv = rpoly.paprvPolyVertices[i_v] = &aprv[i_v];

					// Store the transformed point.
					Assert(!paclpt_points[pi.iShapePoint(i_v)].seteocOut[eocUNINIT]);
					prv->v3Cam = paclpt_points[pi.iShapePoint(i_v)].v3Point;

					// Copy the texture coord.
					prv->tcTex = pi.tcTexCoord(i_v);

					// Set lighting value to negative so we know to light it.
					prv->cvIntensity = -1.0;
				}

				I_STAT_ADD(psVertices, 0, pi.iNumVertices());
			}

			I_STAT_ADD(psVertices, ctmr(), 0);
			I_STAT_ADD(psAllVertices, 0, pi.iNumVertices());

			//
			// Perform lighting.
			//
			if (shape.bIsUnlit())
			{
				// The shape unlit means to light at full brightness.
				if (rpoly.ptexTexture && rpoly.ptexTexture->ppcePalClut)
					rpoly.cvFace = rpoly.ptexTexture->ppcePalClut->pclutClut->cvFromReflect(1.0f);

				// Make sure gouraud shading is off.
				rpoly.seterfFace[erfLIGHT_SHADE] = 0;
			}
			else if (!rpoly.seterfFace[erfLIGHT])
			{
				// No lighting at all.
			}
			// Perform bump-map lighting calculations, unless both texture and bump are disabled.
			else if (shape.bBumpmap() && rpoly.ptexTexture->seterfFeatures[erfBUMP] && rpoly.seterfFace[erfBUMP][erfTEXTURE])
			{
				//
				// For bump mapping, we need to pass additional light info to the rasteriser.
				// The light list returns this for us.
				//
				Assert(rpoly.ptexTexture->ppcePalClut);

				rpoly.Bump = renc.rLightList.bltGetBumpLighting
				(
					*rpoly.ptexTexture->ppcePalClut->pmatMaterial
				);

				if (!rpoly.seterfFace[erfBUMP])
				{
					//
					// If erfBUMP flag is off, then set light direction to Z axis,
					// and change bump intensity based on d3Light incident angle.
					// This effectively disables dynamic bump mapping, rendering the texture
					// as if it were pre-bump mapped.
					//
					rpoly.Bump.lvStrength *= Max(rpoly.Bump.d3Light * pi.d3Normal(), 0);
					rpoly.Bump.d3Light = d3ZAxis;
				}
				else
				{
					// Transform to polygon space.  Use cast to CVector3<>&, and special CDir3 
					// constructor, to avoid renormalise, because our matrix is normalised.
					rpoly.Bump.d3Light = CDir3<>((CVector3<>&)rpoly.Bump.d3Light * pi.mx3ObjToTexture(), true);
					Assert(Abs(rpoly.Bump.d3Light.tZ) <= 1.0f);
				}

				I_STAT_ADD(psFaceLight, ctmr(), 1);
			}
			else
			{
				//
				// Non-bumpmapped surface.
				//

				const CClut* pclut = rpoly.ptexTexture->ppcePalClut->pclutClut;
				Assert(pclut);

				// Optimise for flat surfaces in non-positional lighting.
				if (!pi.bCurved() && !renc.rLightList.bIsPositional())
					rpoly.seterfFace -= erfLIGHT_SHADE;

				if (!rpoly.seterfFace[erfLIGHT_SHADE])
				{
					// Calculate lighting only for the polygon center, as if flat.
					rpoly.cvFace = renc.rLightList.cvGetLighting
					(
						// Pass the face point and face normal.
						pi.v3Point(),
						pi.d3Normal(),
						*pclut
					);

					I_STAT_ADD(psFaceLight, ctmr(), 1);
				}
				else
				{
					// Vertex lighting.
					for (int i_v = 0; i_v < pi.iNumVertices(); i_v++)
					{
						SRenderVertex* prv = rpoly.paprvPolyVertices[i_v];
						if (prv->cvIntensity < 0.0)
						{
							// Light the vertex.
							prv->cvIntensity = renc.rLightList.cvGetLighting
							(
								pi.v3Point(i_v),		// The object-space position.
								pi.d3Normal(i_v),		// The object-space normal.
								*pclut					// Indicates material, scaling.
							);

							I_STAT_ADD(psLight, 0, 1);
						}
					}
				}
			}

			I_STAT_ADD(psLight, ctmr(), 0);

			//
			// If needed, feed the lit polygon to the clipper.
			//
			if (seteoc_poly)
			{
				ESideOf esf = renc.Camera.pbvcamClipVolume()->esfClipPolygonInside
				(
					rpoly, 
					renc.rplhHeap, 
					renc.Camera.campropGetProperties().bPerspective, 
					seteoc_poly
				);

				I_STAT_ADD(psClip, ctmr(), 1);

				if (esf == esfOUTSIDE)
				{
					// Remove this polygon from the heap.
					renc.rplhHeap.darpolyPolygons -= 1;
					continue;
				}
			}

			I_STAT_ADD(psClip, ctmr(), 0);

			//
			// If pSettings->bObjectReject is false for testing purposes, then we are asked to
			// render objects even though they are deemed outside the view.  If in fact a
			// polygon ended up in the view, object rejection is broken.
			//
			Assert(esf_view != esfOUTSIDE);

			// Apply fogging to the polygon, possibly splitting into new polygons.
			if (shape.bTerrain())
			{
				fogTerrainFog.ApplyTerrainFog(rpoly, renc.rplhHeap, renc.Camera.campropGetProperties().bPerspective);
			}
			else if (p_settings->bTargetCache)
			{
				if (p_settings->bHardwareCacheFog)
				{
					rpoly.seterfFace -= erfFOG;
					rpoly.iFogBand = 0;
				}
				else if (rpoly.seterfFace[erfFOG])
				{
					// Find the Z depth for the polygon, and apply fog accordingly.
					float f_y = p_settings->remRemapPosition.fRemapToOriginal
					(
						rpoly.fGetAverageZ()
					);

					// Set the fog level.
					rpoly.iFogBand = fogFog.iGetFogLevel(f_y);
				}
				else
					rpoly.iFogBand = 0;
			}
			else
			{
				fogFog.Apply(rpoly, renc.rplhHeap, renc.Camera.campropGetProperties().bPerspective);
			}

			I_STAT_ADD(psFog, ctmr(), 1);
		}
		
		proProfile.psRenderShape.Add(ctmr_total(), 1);

#if VER_TEST
		// Make sure our commit estimations were valid.
		AlwaysAssert(renc.rplhHeap.darpolyPolygons.uLen		<= u_poly_limit);
		AlwaysAssert(renc.rplhHeap.darppolyPolygons.uLen	<= u_polyp_limit);
#endif
	}
};

//******************************************************************************************
//
// various CShape implementations.
//


	//******************************************************************************************
	void CMesh::Render(const CInstance* pins, CRenderContext& renc, CShapePresence& rsp,
		               const CTransform3<>& tf3_shape_camera, const CPArray<COcclude*>& papoc,
					   ESideOf esf_view) const
	{
		CCycleTimer ctmr;

		CRenderShape<CMesh>(*this, pins, renc, rsp, tf3_shape_camera, papoc, esf_view);

#if (!bINIDIVIDUAL_POLY_STATS)
		psRenderShapeReg.Add(ctmr(), iNumPolygons());
#endif
	}

	//******************************************************************************************
	void CBioMesh::Render(const CInstance* pins, CRenderContext& renc, CShapePresence& rsp,
		               const CTransform3<>& tf3_shape_camera, const CPArray<COcclude*>& papoc,
					   ESideOf esf_view) const
	{
		CCycleTimer ctmr;

		CRenderShape<CBioMesh>(*this, pins, renc, rsp, tf3_shape_camera, papoc, esf_view);
#if (!bINIDIVIDUAL_POLY_STATS)
		psRenderShapeBio.Add(ctmr(), iNumPolygons());
#endif
	}

	//******************************************************************************************
	void NMultiResolution::CQuadRootTINShape::Render(const CInstance* pins, CRenderContext& renc, CShapePresence& rsp,
		                  const CTransform3<>& tf3_shape_camera, const CPArray<COcclude*>& papoc,
						  ESideOf esf_view) const
	{
		CCycleTimer ctmr;

		// For a special-purpose stat, track how many polygons were added for this object.
		uint u_start_polys = renc.rplhHeap.darpolyPolygons.uLen;

		CRenderShape<NMultiResolution::CQuadRootTINShape>(*this, pins, renc, rsp, tf3_shape_camera, papoc, esf_view);

		psTerrain.Add(0, renc.rplhHeap.darpolyPolygons.uLen - u_start_polys);
#if (!bINIDIVIDUAL_POLY_STATS)
		psRenderShapeTrr.Add(ctmr(), renc.rplhHeap.darpolyPolygons.uLen - u_start_polys);
#endif
	}


#if VER_MULTI_RES_WATER
	//******************************************************************************************
	void NMultiResolution::CQuadRootWaterShape::Render(CRenderContext& renc, CShapePresence& rsp,
		                  const CTransform3<>& tf3_shape_camera, const CPArray<COcclude*>& papoc,
						  ESideOf esf_view) const
	{
		// For a special-purpose stat, track how many polygons were added for this object.
		uint u_start_polys = renc.rplhHeap.darpolyPolygons.uLen;

		CRenderShape<NMultiResolution::CQuadRootWaterShape>(*this, renc, rsp, tf3_shape_camera, papoc, esf_view);

		psTerrain.Add(0, renc.rplhHeap.darpolyPolygons.uLen - u_start_polys);
	}
//#if VER_MULTI_RES_WATER
#endif

	//******************************************************************************************
	void CShapeCache::Render(const CInstance*, CRenderContext& renc, CShapePresence& rsp,
		               const CTransform3<>& tf3_shape_camera, const CPArray<COcclude*>& papoc,
					   ESideOf esf_view) const
	{
		CCycleTimer ctmr, ctmr_total;
		int i_v;

		//
		// Safety valve.  Make sure we have enough pipeline heap left to hold this shape.
		//
		if (renc.rplhHeap.darpolyPolygons.uLen + 1 >= renc.rplhHeap.darpolyPolygons.uMax)
			return;
		
		if (rcsRenderCacheSettings.bFreezeCaches)
		{
			CVector3<> v3_cam_shape = renc.Camera.v3Pos() * rsp.pr3GetWorldShape();
			TReal r_plane_adj_dist = plPlane.rDistance(v3_cam_shape) - rPolyPlaneThickness();
			if (CIntFloat(r_plane_adj_dist).bSign() != 0)
				return;
		}

		//
		// Setup.
		//
		#if (VER_DEBUG)
			// Back face cull. Determine if the camera lies to the negative side of the polygon's plane,
			// adjusted by some value for numerical accuracy.

			CVector3<> v3_cam_shape = renc.Camera.v3Pos() * rsp.pr3GetWorldShape();

			TReal r_plane_adj_dist = plPlane.rDistance(v3_cam_shape) - rPolyPlaneThickness();

			// We should never backface cull render-caches.
//			Assert(CIntFloat(r_plane_adj_dist).bSign() == 0);

			I_STAT_ADD(psCull, ctmr(), 1);
		#endif

		I_STAT_ADD(psShapeSetup, ctmr(), 0);

		// Create an array of transformed points for each unique shape point.
		CLArray(CVector3<>, pav3_cam_points, pamvVertices.uLen);

		//
		// Process the polygon points.
		//
		CSet<EOutCode> seteoc_poly;
		CSet<EOutCode> seteoc_poly_all = -CSet<EOutCode>();

	#if (TARGET_PROCESSOR == PROCESSOR_K6_3D && VER_ASM)

		const CBoundVolCamera* pbvcam_clip = renc.Camera.pbvcamClipVolume();

		#define fCAMERA_PLANE_TOLERANCE	1e-4
		const float f_tolerance   = fCAMERA_PLANE_TOLERANCE;
		const float f_toleranceP1 = fCAMERA_PLANE_TOLERANCE + 1.0f;

		const uint u_down_left_masks[2] = {1<<eocDOWN, 1<<eocLEFT};
		const uint u_up_right_masks[2]  = {1<<eocUP,   1<<eocRIGHT};
		const uint u_near_far_masks[2]  = {1<<eocNEAR, 1<<eocFAR};

		const int i_num_verts = pamvVertices.uLen;
		const CTransform3<>* ptf3_mat = &tf3_shape_camera;
		const SVertex* pmv_src_vertex_0 = &pamvVertices[0];
		const CVector3<>* pv3_dest_point_0 = &pav3_cam_points[0];

		typedef CVector3<> tdCVector3;
		typedef CTransform3<> tdCTransform3;
		typedef SVertex tdSVertex;

		double vf_near_far_const;

		// Assumptions made by this code:
		// This code only works if pbvcam points to a CCameraDefPerspectiveNorm!!!
		//   (since seteocOutCodes differs for each type of CBoundVolCamera that 
		//    pbvcam can point to)
		// (fCAMERA_PLANE_TOLERANCE	== 1e-4) to match #define in GeomTypesCamera.cpp
		// ((char *)&pbvcam_clip->CCameraDefPerspectiveNorm.clpNear.rPos - (char *)&pbvcam_clip == 24);
		// ((char *)&pbvcam_clip->CCameraDefPerspectiveNorm.clpFar.rPos - (char *)&pbvcam_clip == 32);
		//   (these three really need to be replaced by proper symbolic references)

		__asm
		{
			femms										;ensure fast switch

			mov		eax,[pbvcam_clip]
			jmp		StartAsm2

			align	16
		StartAsm2:
			movd	mm0,[f_tolerance]

			movd	mm2,[eax+24]						;m2= CCameraDefPerspectiveNorm.clpNear.rPos

			movd	mm3,[eax+32]						;m3= CCameraDefPerspectiveNorm.clpFar.rPos
			movq	mm1,mm0								;m1= f_tolerance

			pfmul	(m0,m2)								;m0= clpNear.rPos * f_tolerance
														;(intentional long decode due to degraded predecode)

			mov		eax,[pmv_src_vertex_0]				;edi= ptr to vertex to be transformed
			pfmul	(m1,m3)								;m1= clpFar.rPos * f_tolerance

			mov		edx,[ptf3_mat]
			pfsub	(m0,m2)								;m0= clpNear.rPos * f_tolerance - clpNear.rPos

			mov		ecx,[i_num_verts]
			pfadd	(m1,m3)								;m1= clpFar.rPos * f_tolerance + clpFar.rPos

			mov		ebx,[pv3_dest_point_0]
			add		edx,8								;force zero disp in first usage of edx below
														;  (to avoid degraded predecode)
			test	ecx,ecx								;check if any points to be transformed
			punpckldq mm0,mm1							;m0= clipFar_const | clipNear_const

			movq	[vf_near_far_const],mm0
			jnz		XformOutcodeLoop					;if there are points to do, go start

			jmp		SkipXformOutcodeLoop				;else skip everything

			// EAX = ptr to first point to be transformed
			// EBX = ptr to first element in array of transformed points
			// ECX = number of points to be transformed
			// EDX = ptr to transform matrix

			align	16
			nop											;establish 3 byte starting code offset
			nop
			nop
		XformOutcodeLoop:
			movd	mm0,[eax]tdSVertex.v3Point.tX		;m0= X
			nop											;1-byte NOOP to avoid degraded predecode 
														;  and maintain decode pairing

			movd	mm1,[eax]tdSVertex.v3Point.tY		;m1= Y
			nop											;1-byte NOOP to avoid degraded predecode
														;  and maintain decode pairing

			movd	mm2,[eax]tdSVertex.v3Point.tZ		;m2= Z
			punpckldq mm0,mm0							;m0= X | X

			movd	mm3,[edx-8]tdCTransform3.mx3Mat.v3X.tZ	;m3= m02
			punpckldq mm1,mm1								;m1= Y | Y

			movd	mm4,[edx-8]tdCTransform3.mx3Mat.v3Y.tZ	;m4= m12
			punpckldq mm2,mm2								;m2= Z | Z

			movd	mm5,[edx-8]tdCTransform3.mx3Mat.v3Z.tZ	;m5= m22
			pfmul	(m3,m0)									;m3= m02*X

			movd	mm6,[edx-8]tdCTransform3.v3Pos.tZ		;m6= m32
			pfmul	(m4,m1)									;m4= m12*Y

			movq	mm7,[edx-8]tdCTransform3.mx3Mat.v3X.tX	;m7= m01 | m00
			pfmul	(m5,m2)									;m5= m22*Z

			pfadd	(m4,m3)									;m4= m02*X + m12*Y
			movq	mm3,[edx-8]tdCTransform3.mx3Mat.v3Y.tX	;m3= m11 | m10

			pfadd	(m6,m5)									;m6= m22*Z + m32
			pfmul	(m7,m0)									;m7= m01*X | m00*X

			movq	mm5,[edx-8]tdCTransform3.mx3Mat.v3Z.tX	;m5= m21 | m20
			pfmul	(m3,m1)									;m3= m11*Y | m10*Y

			pfadd	(m6,m4)									;m6=  0  | resultZ
			movq	mm4,[edx-8]tdCTransform3.v3Pos.tX		;m4= m31 | m30

			movd	[ebx]tdCVector3.tZ,mm6
			pfmul	(m5,m2)									;m5= m21*Z | m20*Z

			pfadd	(m7,m3)									;m7= m01*X + m11*Y | m00*X + m10*Y
			movd	mm0,[f_toleranceP1]						;m0= f_tolerance + 1

			pfadd	(m5,m4)									;m5= m21*Z + m31   | m20*Z + m30
			movq	mm1,[vf_near_far_const]					;m1= clipFar_const | clipNear_const

			lea		eax,[eax + SIZE tdSVertex]				;advance to next vertex to be transformed
			movq	mm4,[u_down_left_masks]					;m4= eocLEFT | eocDOWN

			pfadd	(m7,m5)									;m7=    resultY    | resultX
			movq	[ebx]tdCVector3.tX,mm7

			test	ebx,ebx							;2-byte NOOP to avoid degraded predecode
			movq	mm5,[u_near_far_masks]			;m5=  eocFAR | eocNEAR

			punpckldq mm6,mm7						;m6= X | Z
			psrlq	mm7,32							;m7= 0 | Y

			movq	mm2,mm7							;m2= 0 | Y
			psllq	mm7,32							;m7= Y | 0

			pfsubr	(m7,m2)							;m7= -Y | Y
			nop										;1-byte NOOP to avoid degraded predecode 

			pfmul	(m2,m0)							;m2= Y*f_toleranceP1
			nop										;1-byte NOOP to avoid degraded predecode 

			movq	mm0,[u_up_right_masks]			;m0= eocRIGHT | eocUP
			pfadd	(m7,m1)							;m7= clipFar_const-Y | clipNear_const+Y

			punpckldq mm2,mm2						;m2= Y*f_toleranceP1 | Y*f_toleranceP1
			cmp		ebx,0							;3-byte NOOP to avoid degraded predecode

			movq	mm3,mm2							;m3= Y*f_toleranceP1 | Y*f_toleranceP1
			pfadd	(m2,m6)							;m2= Y*f_toleranceP1+X | Y*f_toleranceP1+Z

			psrad	mm7,31							;m7= sign(fFar) | sign(fNear)
			pfsub	(m3,m6)							;m3= Y*f_toleranceP1-X | Y*f_toleranceP1-Z

			psrad	mm2,31							;m2= sign(fLeft) | sign(fDown)
			pand	mm7,mm5							;m7= eocFAR? | eocNEAR?

			psrad	mm3,31							;m3= sign(fRight) | sign(fUp)
			pand	mm2,mm4							;m2= eocLEFT? | eocDOWN?

			por		mm7,mm2							;m7= accumulate eoc's
			pand	mm3,mm0							;m3= eocRIGHT? | eocUP?

			por		mm7,mm3							;m7= accumulate eoc's
			dec		ecx								;decrement loop counter

			movq	mm6,mm7							;m6= copy of low eoc accumulation
			punpckhdq mm7,mm7						;m7= copy of hight eoc accumulation

			movd	mm0,[seteoc_poly]
			por		mm7,mm6							;m7= accumulation of all six eoc's

			movd	mm1,[seteoc_poly_all]
			jz		ExitXformOutcodeLoop			;if not done, go do next point

			por		mm0,mm7							;m0= updated OR accumulation of outcodes
			movd	[seteoc_poly],mm0

			pand	mm1,mm7							;m1= updated AND accumulation of outcodes
			movd	[seteoc_poly_all],mm1

			add		ebx,SIZE tdCVector3				;advance to next transformed point
			jmp		XformOutcodeLoop				;go do next point

			align	16
		ExitXformOutcodeLoop:
			por		mm0,mm7							;m0= updated OR accumulation of outcodes
			movd	[seteoc_poly],mm0

			pand	mm1,mm7							;m1= updated AND accumulation of outcodes
			movd	[seteoc_poly_all],mm1

		SkipXformOutcodeLoop:
			femms									;empty MMX state and ensure fast switch
		}

		I_STAT_ADD(psView, 0, pamvVertices.uLen);

	#else // if (TARGET_PROCESSOR == PROCESSOR_K6_3D && VER_ASM)

		for (i_v = 0; i_v < pamvVertices.uLen; i_v++)
		{
			//
			// Transform and outcode the points.
			//
			// Store transformed point in array.
			pav3_cam_points[i_v] = pamvVertices[i_v].v3Point * tf3_shape_camera;

			// Also generate outcode.
			CSet<EOutCode> seteoc =
				renc.Camera.pbvcamClipVolume()->seteocOutCodes(pav3_cam_points[i_v]);

			// Combine outcodes of all points.
			seteoc_poly     += seteoc;
			seteoc_poly_all &= seteoc;
		}

		I_STAT_ADD(psView, 0, pamvVertices.uLen);

	#endif // (TARGET_PROCESSOR == PROCESSOR_K6_3D) && VER_ASM

		//
		// Perform a quick intersection test with the camera volume.
		// If seteoc_poly == 0, the polygon is entirely inside the view volume, and
		// no clipping will be done later.
		// Otherwise, the polygon may intersect one or more planes, and must clip it later.
		//

		I_STAT_ADD(psView, ctmr(), 0);

		// See whether we can reject the polygon.
		if (seteoc_poly_all)
		{
			// All points are outside at least one plane.

			#if (!bINIDIVIDUAL_POLY_STATS)
				psRenderShapeImc.Add(ctmr(), 1);
			#endif

			return;
		}

		//
		// Attempt to occlude the polygon.
		//
		if (COcclude::bUsePolygonOcclusion && papoc.uLen)
		{
#if (0)			
			// Skip this polygon if it is occluded.
			if (bOccludePolygon(papoc, pav3_cam_points))
			{
				#if (!bINIDIVIDUAL_POLY_STATS)
					psRenderShapeImc.Add(ctmr(), 1);
				#endif

				return;
			}
#else
			// Iterate through the occlusion objects looking for occlusion or intersection.
			for (uint u = 0; u < papoc.uLen; ++u)
			{
				//
				// If the points are entirely inside the occluding object's bounding volume,
				// the polygon is occluded.
				//

				if (papoc[u]->bInsideNormPlanes(pav3_cam_points))
				{
					// Occluded.

					#if (!bINIDIVIDUAL_POLY_STATS)
						psRenderShapeImc.Add(ctmr(), 1);
					#endif

					conocOcclusion.AddOccludedPoly();
					return;
				}
			}
#endif
		}

		//
		// Move the polygon forward, to improve render cache sorting.
		// Must be done after occlusion.
		//

	#if (TARGET_PROCESSOR == PROCESSOR_K6_3D && VER_ASM)

		typedef CShapeCache tdCShapeCache;
		typedef CTransform3<> tdCTransform3;
		typedef CVector3<> tdCVector3;

		CVector3<> v3_actual;
		CVector3<> v3_near;

		const float f_scale_min = 0.002f;

		__asm
		{
			femms												;ensure fast switch

			mov		ecx,[this]
			jmp		StartAsm4

			align	16
			nop													;establish 2 byte starting code offset
			nop
		StartAsm4:
			mov		eax,[ptf3_mat]

			movd	mm3,[ecx]tdCShapeCache.v3ControlActual.tX	;m3= actlX

			movd	mm6,[ecx]tdCShapeCache.v3ControlNear.tX		;m6= nearX

			movd	mm4,[ecx]tdCShapeCache.v3ControlActual.tY	;m4= actlY

			movd	mm7,[ecx]tdCShapeCache.v3ControlNear.tY		;m7= nearY
			punpckldq mm3,mm6									;m3= nearX | actlX

			movd	mm5,[ecx]tdCShapeCache.v3ControlActual.tZ	;m5= actlZ

			movd	mm6,[ecx]tdCShapeCache.v3ControlNear.tZ		;m6= nearZ
			punpckldq mm4,mm7									;m4= nearY | actlY

			movd	mm0,[eax]tdCTransform3.mx3Mat.v3X.tY		;m0= m02
			mov		ebx,0										;5-byte NOOP to avoid degraded predecode 

			movd	mm1,[eax]tdCTransform3.mx3Mat.v3Y.tY		;m1= m12
			punpckldq mm5,mm6									;m5= nearZ | actlZ

			movd	mm2,[eax]tdCTransform3.mx3Mat.v3Z.tY		;m2= m22
			punpckldq mm0,mm0									;m0= m02 | m02

			punpckldq mm1,mm1									;m1= m12 | m12
			movd	mm7,[eax]tdCTransform3.v3Pos.tY				;m7= m32
			
			pfmul	(m0,m3)									;m0= m02*nearX | m02*actlX
			punpckldq mm2,mm2								;m2= m22 | m22

			pfmul	(m1,m4)									;m1= m12*neaY | m12*actlY

			pfmul	(m2,m5)									;m2= m22*nearZ | m22*actlZ
			punpckldq mm7,mm7								;m7= m32 | m32

 			mov		ecx,[i_num_verts]
			pfadd	(m0,m1)									;m0= m02*nearX + m12*neaY | m02*actlX + m12*actlY

			mov		eax,[pv3_dest_point_0]
			pfadd	(m2,m7)									;m2= m22*nearZ + m32| m22*actlZ + m32

			movd	mm4,[f_scale_min]						;m4= 0.002f

			pfadd	(m0,m2)									;m0= v3_near.tY | v3_actual.tY
			movd	ebx,mm0									;m0= v3_actual.tY

			pfrcp	(m1,m0)									;m1= 1/v3_actual.tY | 1/v3_actual.tY
			punpckhdq mm0,mm0								;m0=     v3_near.tY | v3_near.tY

			test	ebx,0x7FFFFFFF							;check if (v3_actual.tY == 0.0f)
			jz		SkipScaling								;skip scaling if scale doesn't exist

			pfmul	(m0,m1)									;m0= f_scale
			nop												;1-byte NOOP to avoid degraded predecode 

			test	ecx,ecx									;check if any points to be transformed
			pfmax	(m0,m4)									;m0= (f_scale < 0.002f) ? 0.002f : f_scale

			jz		SkipScaling

			// EAX = ptr to first element in array of points to be scaled
			// ECX = number of points to be transformed

			nop												;1-byte NOOP to avoid degraded predecode 
		ScaleLoop:
			movd	mm3,[eax]tdCVector3.tZ					;m3= 0 | Z
			movq	mm2,[eax]tdCVector3.tX					;m2= Y | X

			add		eax,SIZE tdCVector3						;advance to ptr to next vertex to be transformed
			pfmul	(m3,m0)									;m3=     0     | Z*f_scale

			movd	[eax - SIZE tdCVector3]tdCVector3.tZ,mm3
			pfmul	(m2,m0)									;m2= Y*f_scale | Z*f_scale

			nop												;1-byte NOOP to avoid degraded predecode 
			movq	[eax - SIZE tdCVector3]tdCVector3.tX,mm2

			loop	ScaleLoop								;decrement loop counter; 
															;if not done, go do next point
		SkipScaling:
			femms											;empty MMX state and ensure fast switch
		}

	#else // if (TARGET_PROCESSOR == PROCESSOR_K6_3D && VER_ASM)

		CVector3<> v3_actual = v3ControlActual * tf3_shape_camera;
		CVector3<> v3_near   = v3ControlNear   * tf3_shape_camera;

		// If a scale exists, apply the scale to all the points.
		if (v3_actual.tY != 0.0f)
		{
			// Compute the scale.
			float f_scale = v3_near.tY / v3_actual.tY;
			if (f_scale < 0.002f)
				f_scale = 0.002f;

			for (i_v = 0; i_v < pamvVertices.uLen; i_v++)
				pav3_cam_points[i_v] *= f_scale;
		}

	#endif // else


		//
		// Create a new render polygon on the array.
		//

		// Use paAlloc() rather than new, to bypass the default constructor.
		// Call InitFast() instead.
		CRenderPolygon& rpoly = *renc.rplhHeap.darpolyPolygons.paAlloc(1);
		rpoly.InitFast();
		rpoly.bPrerasterized = false;
			
		rpoly.ptexTexture = ptexTexture.ptGet();
		Assert(rpoly.ptexTexture);

		// Find the intersection of the global and polygon render features.
		rpoly.seterfFace = ptexTexture->seterfFeatures & renc.Renderer.pSettings->seterfState;
		rpoly.eamAddressMode = eamTileNone;

		I_STAT_ADD(psPolygons, ctmr(), 1);

		//
		// Allocate the polygon vertices.
		//

		// Vertex pointers must be allocated on the vertex pointer heap.
		rpoly.paprvPolyVertices = renc.rplhHeap.daprvVertices.paAlloc(pamvVertices.uLen);

		// Unique vertices.  Allocate all at once.
		SRenderVertex* aprv = renc.rplhHeap.darvVertices.paAlloc(pamvVertices.uLen);

		// Init them.
		for (i_v = 0; i_v < pamvVertices.uLen; i_v++)
		{
			SRenderVertex* prv = rpoly.paprvPolyVertices[i_v] = &aprv[i_v];

			// Store the transformed point.
			prv->v3Cam = pav3_cam_points[i_v];

			// Copy the texture coord.
			prv->tcTex = pamvVertices[i_v].tcTex;
		}

		I_STAT_ADD(psVertices, ctmr(), pamvVertices.uLen);

		//
		// If needed, feed the lit polygon to the clipper.
		//
		if (seteoc_poly)
		{
			ESideOf esf = renc.Camera.pbvcamClipVolume()->esfClipPolygonInside
			(
				rpoly, 
				renc.rplhHeap, 
				renc.Camera.campropGetProperties().bPerspective, 
				seteoc_poly
			);

			I_STAT_ADD(psClip, ctmr(), 1);

			if (esf == esfOUTSIDE)
			{
				// Remove this polygon from the heap.
				renc.rplhHeap.darpolyPolygons -= 1;

				#if (!bINIDIVIDUAL_POLY_STATS)
					psRenderShapeImc.Add(ctmr(), 1);
				#endif

				return;
			}
		}

#if (!bINIDIVIDUAL_POLY_STATS)
		psRenderShapeImc.Add(ctmr(), 1);
#endif

		proProfile.psRenderShape.Add(ctmr_total(), 1);
	}


//**********************************************************************************************
//
// CRenderContext implementation.
//

	bool CRenderContext::bRenderTriggers = false;

	//******************************************************************************************
	CRenderContext::CRenderContext
	(
		const CRenderer&	ren,
		CPipelineHeap&		rplh,
		const CCamera&		cam,
		CLightList&			rltl
	) :
		Renderer(ren), rplhHeap(rplh), Camera(cam), rLightList(rltl),
		tf3ToNormalisedCamera(cam.tf3ToNormalisedCamera()), bTargetHardware(false),
		uNumPolysPrerasterized(0), uNumVertsProjected(0)
	{
	}

//******************************************************************************************
//
// CRenderer implementation.
//

	//**********************************************************************************************
	//
	// CRenderer::SSettings implementation.
	//

		//******************************************************************************************
		CRenderer::SSettings::SSettings() :
			bObjectReject(true), 
			bObjectAccept(true), 
			bRenderCache(true), 
			bDetailReduce(true),
			fDetailReduceFactor(1.0),
			bShadow(false),
			esSortMethod(esDepthSort),
			bTargetCache(false),
			bBackfaceCull(true),
			bTerrainHeightRelative(false),
			bRenderStaticObjects(true),
			bRenderMovingObjects(true),
			bExecuteScheduler(true),
			bUseDistanceCulling(true),
			bHardwareCacheFog(false)
		{
		}

	//**********************************************************************************************
	CRenderer::CRenderer(CScreenRender* psr, SSettings* pset)
		: pScreenRender(psr), pSettings(pset)
	{
		// Also make the screen renderer point to its portion of these settings.
		psr->pSettings = static_cast<CScreenRender::SSettings*>(pset);
	}

	//******************************************************************************************
	CRenderer::~CRenderer()
	{
	}

	//**********************************************************************************************
	inline void CRenderer::AdjustPresence(CPresence3<>& pr3, const CPartition* ppart, const CCamera& cam) const
	{
		if (ppart->bGetAlwaysFace())
		{
			// Get the facing vector.
			CVector3<> v3 = cam.v3Pos() - pr3.v3Pos;

			// Blow off changes in Z.
			v3.tZ = 0.0f;

			// Prevent undefined behaviour.
			if (v3.tX == 0.0f && v3.tY == 0.0f)
			{
				v3.tY = 1.0f;
			}

			// Rotate the presence.
			pr3.r3Rot = CRotate3<>(d3YAxis, v3);
			return;
		}

		if (pSettings->bTerrainHeightRelative)
		{
			// Set the object's height to that relative to the terrain.
			CWDbQueryTerrain wqtrr;

			if (wqtrr.tGet())
			{
				// Move height.
				TReal r_height = wqtrr.tGet()->rHeightTIN(pr3.v3Pos.tX, pr3.v3Pos.tY);
				pr3.v3Pos.tZ -= r_height;
			}
		}
	}

	//******************************************************************************************
	void CRenderer::RenderPartition
	(
		CRenderContext&			renc,
		const CPresence3<>&		pr3_cam_inv,
		CPartition*             ppart,
		CPArray<COcclude*>		papoc,
		ESideOf					esf_view
	)
	{
		CCycleTimer	ctmr_total, ctmr;

		Assert(ppart);
		Assert(!ptCast<CTerrain>(ppart));

		// Check the visible flag.
		if (!ppart->bIsVisible())
			return;

		// Check if the partition priority suggests continuing.
		if (!ppart->bIsWithinPriority())
			return;

		// Get the distance square.
		float f_distance_sqr = ppart->fDistanceFromGlobalCameraSqr();

		// Use distance culling.
		if (pSettings->bUseDistanceCulling)
		{
			if (!ppart->bInRange(f_distance_sqr))
				return;
		}
		else
		{
			if (!ppart->bInRangeShadow(f_distance_sqr))
				return;
		}

		// Do something only if we have a shape, or some children.
		rptr_const<CShape> psh_shape = ppart->pshGetShape();
		if (psh_shape)
		{
			// Create a set of properties for the renderer.
			CSet<EPartitionProperties> setepp;
			if (ppart->pdGetData().bCacheable)
				setepp += eppCACHEABLE;
			if (ppart->pdGetData().bCastShadow)
				setepp += eppCASTSHADOW;

			// Check required and forbidden attribute bits.
			if (!setepp.bAll(pSettings->seteppRequired) ||
				(setepp & pSettings->seteppForbidden))
				psh_shape = rptr0;

			// Do the flags say we should eliminate moving and/or static objects?
			else if (!pSettings->bRenderStaticObjects && !ppart->bIsMoving())
				psh_shape = rptr0;
			else if (!pSettings->bRenderMovingObjects &&  ppart->bIsMoving())
				psh_shape = rptr0;
		}

		if (!psh_shape && !ppart->ppartChildren())
			return;

		// Copy the occlusion object array.
		CLArray(COcclude*, papoc_intersect, papoc.uLen);
		for (int i = 0; i < papoc.uLen; i++)
			papoc_intersect[i] = papoc[i];

		// Get the shape to normalized camera space transform.
		CPresence3<> pr3_part = ppart->pr3Presence();
		psIntersectSetup.Add(ctmr(), 1);

		AdjustPresence(pr3_part, ppart, renc.Camera);
		psIntersectAdjust.Add(ctmr(), 1);

		CTransform3<> tf3_shape_camera = pr3_part * renc.tf3ToNormalisedCamera;
		psIntersectSetup.Add(ctmr());

		//
		// Test intersection. If the object is a sprite object, mark the object as intersecting
		// and pass it directly to the shape renderer.
		//
		if (ppart->ppartChildren() || ppart->bTestBoxIntersection())
		{
			//
			// Only check intersection if the partition has a bounding volume.
			//

			// Get a pointer to the bounding volume of the partition.
			const CBoundVol* pbv_it = ppart->pbvBoundingVol();

			if (pbv_it && pbv_it->ebvGetType() != ebvINFINITE)
			{
				//
				// Assume that the partition is inside the camera's view if its parent is inside
				// and there are no occluding objects.
				//
				if (esf_view != esfINSIDE || papoc.uLen)
				{
					// Test for intersection or containment if required.
					const CBoundVolBox* pbvb_box_it = pbv_it->pbvbCast();

					if (pbvb_box_it)
					{
						// Get a pointer to the bounding volume of the camera.
						const CBoundVol* pbv_cam = renc.Camera.pbvcamClipVolume();
						Assert(pbv_cam);

						// Construct the box to camera transform.
						CTransform3<> tf3_box;

						if (pSettings->bTerrainHeightRelative && ppart->bIsPureSpatial())
						{
							// Adjust the bounding box of the partition such that the minimum Z coordinate is at
							// rPROJ_PLANE_MIN_Z.
							const TReal rPROJ_PLANE_MIN_Z = -1.0f;

							CVector3<> v3_part_pos = ppart->v3Pos();
							CVector3<> v3_bvb_max  = (*pbvb_box_it)[0];

							// Adjust the partition's Z position and bounding box Z extent.
							if (v3_part_pos.tZ > 0)
							{
								Assert(Fuzzy(ppart->pr3Presence().rScale) == 1.0f);

								v3_part_pos.tZ = (v3_part_pos.tZ + v3_bvb_max.tZ + rPROJ_PLANE_MIN_Z) * .5f;
								v3_bvb_max.tZ  =  v3_part_pos.tZ - rPROJ_PLANE_MIN_Z;
							}

							// Create a transform to normalized camera space for this adjusted bounding box.
							CTransform3<> tf3_adj_part_camera = renc.tf3ToNormalisedCamera;
							tf3_adj_part_camera.v3Pos = v3_part_pos * renc.tf3ToNormalisedCamera;

							// Construct the box to camera transform. This code is copied from the
							// bounding box implementation to avoid its constructor call overhead.
							tf3_box.mx3Mat.v3X = tf3_adj_part_camera.mx3Mat.v3X * (v3_bvb_max.tX * 2);
							tf3_box.mx3Mat.v3Y = tf3_adj_part_camera.mx3Mat.v3Y * (v3_bvb_max.tY * 2);
							tf3_box.mx3Mat.v3Z = tf3_adj_part_camera.mx3Mat.v3Z * (v3_bvb_max.tZ * 2);
							tf3_box.v3Pos = -v3_bvb_max * tf3_adj_part_camera;
						}
						else
							tf3_box = pbvb_box_it->tf3Box(tf3_shape_camera);


						// Do the camera bounding volume test.
						if (esf_view != esfINSIDE)
							// Test if the partition is inside, intersecting with or outside the camera's view.
							esf_view = pbv_cam->esfSideOf(tf3_box);

						psIntersectCam.Add(ctmr(), 1);

						if (esf_view != esfOUTSIDE && COcclude::bUseObjectOcclusion)
						{
							// Do the occlusion test.
							if (bOccludePartitionNorm(papoc, papoc_intersect, tf3_box))
								esf_view = esfOUTSIDE;
							psIntersectOcclude.Add(ctmr(), 1);
							conocOcclusion.AddOccludedPartition();
						}
					}
					else
					{
						// Currently no occlusion for non-box volumes.
						if (esf_view != esfINSIDE)
						{
							// Get a pointer to the bounding volume of the camera.
							const CBoundVol* pbv_cam = renc.Camera.pbvBoundingVol();
							Assert(pbv_cam);

							// Construct the partition-to-camera transform.
							CPresence3<> pr3_it_cam = pr3_part * pr3_cam_inv;

							//
							// Do the camera bounding volume test.
							//

							//
							// Rather than using the general partition intersection function, manually call the
							// equivalent bounding volume functions, passing only a single combined presence.
							// This avoids an extra presence invert per intersection test. The camera effectively
							// has null presence.
							//
							// Test if the partition is inside, intersecting with or outside the camera's view.
							esf_view = pbv_cam->esfSideOf(*pbv_it, 0, &pr3_it_cam);
						}
						psIntersectCam.Add(ctmr(), 1);
					}

					//
					// If the partition is outside the view of the camera, or if the partition is
					// occluded, reject it
					//
					if (prenMain->pSettings->bObjectReject && esf_view == esfOUTSIDE)
					{
						psIntersect.Add(ctmr_total(), 1);
						return;
					}
				}
			}
			else
			{
				esf_view = esfINTERSECT;
			}
		}

		// Build the shape transform.
		CShapePresence rsp(pr3_part, renc.Camera.pr3GetPresence());

		//
		// Schedule builds except if the destination is a render cache.
		//
		if (!pSettings->bTargetCache)
		{
			// Try to render cache the current node; if it succeeds, render the cache.
			if (pSettings->bRenderCache && bShouldCache
			(
				ppart,
				renc.Camera,
				f_distance_sqr,
				rsp
			))
			{
				ppart->prencGet()->SetVisible(true);
				new (shcScheduler) CScheduleCache
				(
					shcScheduler,	// Scheduler used for scheduling caches.
					ppart,			// Cached partition.
					renc,			// Render context.
					papoc_intersect	// List of intersecting occlusion objects.
				);
				ppart->prencGet()->UpdateFrameKey();

				psCacheSched.Add(ctmr(), 1);

				psIntersect.Add(ctmr_total(), 1);

				// If the current node was render cached, it is atomic; return without recursing.
				return;
			}

			psCacheSched.Add(ctmr(), 1);
		}

		psIntersect.Add(ctmr_total(), 1);

		// If this node has a shape, render it.
		if (psh_shape)
		{
			// Select shape version of appropriate level.
			if (pSettings->bTerrainHeightRelative)
			{
				psh_shape = psh_shape->pshGetTerrainShape();
			}
			else
			{
				if (pSettings->bDetailReduce)
				{
					float f_screen_size = ppart->fEstimateScreenSize
					(
						*renc.Camera.pcamGetParent(),
						f_distance_sqr
					);

					psh_shape = psh_shape->pshGetProperShape(f_screen_size);
				}
			}

			if (!pSettings->bObjectAccept || ppart->bGetAlwaysFace())
				// Disable trivial acceptance.
				esf_view |= esfOUTSIDE;

			psIntersectDetail.Add(ctmr(), 1);

			//
			// Note:
			//		This is a hack; the water should never be prerendered twice in a given scene no
			//		matter what. This code assumes that this is the right thing to do for every
			//		object that has a pre-render behaviour.
			//		
			if (!pSettings->bTargetCache)
				ppart->PreRender(renc);

			psIntersectPreRender.Add(ctmr(), 1);

			psIntersect.Add(ctmr_total());

			psh_shape->Render
			(
				ptCast<CInstance>(ppart),
				renc,
				rsp,
				tf3_shape_camera,
				papoc_intersect,
				esf_view
			);
		}

		//
		// Render any children.
		//
		CPartition* ppartc = ppart->ppartChildren();
		if (ppartc)
		{
			// Iterate through the children and call this function recursively.
			for (CPartition::iterator it = ppartc->begin(); it != ppartc->end(); ++it)
				RenderPartition(renc, pr3_cam_inv, *it, papoc_intersect, esf_view);
		}
	}

	//******************************************************************************************
	void CRenderer::RenderScene(const CCamera& cam, CLightList& rltl_lights,
			                    CPartition* ppart_scene, const CPArray<COcclude*>& rpapoc,
								ESideOf esf_view, CPartition* ppart_terrain)
	{
		CCycleTimer ctmr_total;

		CPipelineHeap plh;	// Local version of the pipeline heap.

		// Create the rendering context for this scene.
		CRenderContext renc(*this, plh, cam, rltl_lights);

		// Set hardware target flag.
		pScreenRender->SetHardwareOut(!pSettings->bTargetCache);
		renc.bTargetHardware = pScreenRender->bTargetHardware();

		pScreenRender->ClearMemSurfaces();
		proProfile.psClearScreen.Add(ctmr_total());

		//
		// Note:
		//		For some reason the scheduler does not execute correctly with the terrain
		//		rendering step after the RenderPartition call. The current code works for
		//		now, but attention should be given to the interaction between the scheduler
		//		and the terrain when adding terrain to the scheduler.
		//

		// Handle separate terrain object specially.
		if (ppart_terrain)
		{
			// If there is a shape, render it.
			rptr_const<CShape> psh_shape = ppart_terrain->pshGetShape();
			CShapePresence rsp(ppart_terrain->pr3Presence(), renc.Camera.pr3GetPresence());
			if (psh_shape)
			{
				psh_shape->Render
				(
					ptCast<CInstance>(ppart_terrain),
					renc,
					rsp,
					ppart_terrain->pr3Presence() * renc.tf3ToNormalisedCamera,
					rpapoc,
					esfINTERSECT
				);
				RasteriseZBufferTerrain(renc);
			}
		}

		/*
		// Store the camera's properties.
		CCamera::SProperties camprop = cam.campropGetProperties();
		CCamera::SProperties camprop_new = camprop;

		// Move the far clipping plane to the position of the last fog band.
		AlwaysAssert(bWithin(fogFog.fFogLastBand, 0.001f, 1.0f));
		if (!pSettings->bTargetCache)
			camprop_new.rFarClipPlaneDist *= fogFog.fFogLastBand;
		((CCamera&)cam).SetProperties(camprop_new);
		*/

		// Recurse through partitions, rendering instances as found.
		RenderPartition(renc, ~cam.pr3GetPresence(), ppart_scene, rpapoc, esf_view);

		// Render the triggers if required.
		if (renc.bRenderTriggers && !pSettings->bTargetCache)
		{
			RenderPartition
			(
				renc,
				~cam.pr3Presence(),
				wWorld.ppartTriggerPartitionList(),
				rpapoc,
				esf_view
			);
		}

		// Execute scheduled operations immediately if required
		if (pSettings->bExecuteScheduler && !pSettings->bTargetCache)
		{
			CCycleTimer cmtr_caches;

			// Add unseen caches to the scheduler.
			if (pSettings->bRenderCache)
			{
				CCycleTimer ctmr;

				AddUnseenCaches(shcScheduler, cam, renc);

				psUnseenCaches.Add(ctmr(), 1);
			}

			{
				// Add unused caches to the LRU.
				renclRenderCacheList.AddToLRU();
				shcScheduler.Execute();
				UploadCaches();
				psExecuteCaches.Add(cmtr_caches(), 1);
			}
		}

		// Exit rendering if there is nothing to rasterize.
		if (!pSettings->bExecuteScheduler && !renc.rplhHeap.parpolyPolygons().uLen)
		{
			return;
		}

		// Restore the camera's properties.
		//((CCamera&)cam).SetProperties(camprop);
		// Rasterize scene.
		RasteriseScene(renc);

		proProfile.psRender.Add(ctmr_total(), 1);
	}

	//******************************************************************************************
	void CRenderer::RenderScene(const CCamera& cam, const std::list<CInstance*>& listins_lights,
			                    CPartition* ppart_scene, const CPArray<COcclude*>& rpapoc,
								ESideOf esf_view, CPartition* ppart_terrain)
	{
		//
		// Build a CLightList from the light list.
		//
		CCycleTimer ctmr;

		// Compiler bug: MSVC 4.2 gives an internal compiler error if pltl_lights is a 
		// variable rather than a pointer.
		aptr<CLightList> pltl_lights = new CLightList(listins_lights);

		if (pSettings->seterfState[erfLIGHT])
		{
			// Update the primary bump table with current lighting data.
			pSettings->bltPrimary = pltl_lights->bltGetPrimaryBumpLighting();

			//
			// Update any shadow buffers needed.
			//
			if (!pSettings->bShadow)
				// Turn off the shadows.
				pltl_lights->UpdateShadows(0);
			else
			{
				// Turn on the shadows.  To do: get terrain into partitioning volume as well.
				pltl_lights->UpdateShadows(ppart_scene);
			}
		}

		proProfile.psRender.Add(ctmr());

		// Render the scene with this list.
		RenderScene(cam, *pltl_lights, ppart_scene, rpapoc, esf_view, ppart_terrain);
	}

	//**********************************************************************************************
	void CRenderer::RenderScene(const CCamera& cam, const TPartitionList& listpart_shapes)
	{
		//
		// Notes:
		//		This RenderScene is called for rendering terrain objects.
		//
		CCycleTimer ctmr, ctmr_total;

		CPipelineHeap plh;			// Local version of the pipeline heap.

		// Create the rendering context for this scene.
		CLightList ltl{ std::list<CInstance*>() };

		CRenderContext renc(*this, plh, cam, ltl);

		pScreenRender->ClearMemSurfaces();
		proProfile.psClearScreen.Add(ctmr_total());

		// Loop through partitions, rendering each one.
		CPresence3<> pr3_cam_inv = ~cam.pr3GetPresence();
		forall_const (listpart_shapes, TPartitionList, itpart)
		{
			// If this node has a shape, render it.
			CInstance* pins = ptCast<CInstance>((*itpart).ppart);
			if (pins)
			{
				rptr_const<CShape> psh_shape = ptCastRenderType<CShape>(pins->prdtGetRenderInfo());
				if (psh_shape)
				{
					if (pSettings->bUseDistanceCulling)
					{
						float f_distance_sqr = pins->fDistanceFromGlobalCameraSqr();
						if (!pins->bInRange(f_distance_sqr))
							continue;
					}

					// Select shape version of appropriate level.  
					if (pSettings->bDetailReduce && psh_shape->pshCoarser)
					{
						float f_screen_size = (*itpart).ppart->fEstimateScreenSize(renc.Camera);
						psh_shape = psh_shape->pshGetProperShape(f_screen_size);
					}

					Assert(!ptCast<CTerrain>((*itpart).ppart));

					// Dummy array of occlusion objects.
					CLArray(COcclude*, papoc, 0);

					CPresence3<> pr3_part = (*itpart).ppart->pr3Presence();
					AdjustPresence(pr3_part, (*itpart).ppart, cam);

					CShapePresence rsp(pr3_part, renc.Camera.pr3GetPresence());

					psh_shape->Render
					(
						pins,
						renc,
						rsp,
						pr3_part * renc.tf3ToNormalisedCamera,
						papoc,
						(*itpart).esfView
					);
				}
			}
		}

		RasteriseScene(renc);

		proProfile.psRender.Add(ctmr_total(), 1);
	}

	//**********************************************************************************************
	void CRenderer::RasteriseScene(CRenderContext& renc)
	{
		CCycleTimer ctmr;

		// Use hardware batch rasterization if possible.
		if (pScreenRender->bTargetHardware())
		{
			RasteriseZBufferBatch(renc);
			return;
		}

		uint u_poly;

		// Add particles.
		if (pScreenRender->bTargetMainScreen())
			Particles.Add(renc.Camera, renc.rplhHeap);

		// Construct arrays which reference only the portions of the arrays built this function.
		CPArray<CRenderPolygon> parpoly	= renc.rplhHeap.parpolyPolygons();
		CPArray<SRenderVertex>  parv    = renc.rplhHeap.parvVertices();

		//
		// Project the vertices in the RenderVertex array.
		//
		renc.Camera.ProjectVertices(parv);
		psProject.Add(ctmr(), parv.uLen);

		// Determine the screen area of the polygons for feature reduction.
		if (d3dDriver.bUseD3D())
			fMaxAreaCullTerrain = 0.000005f;
		else
			fMaxAreaCullTerrain = fMaxAreaCull;

		// Set the target device.
		srd3dRenderer.SetOutputFlag(false);

		pScreenRender->BeginFrame();

		// Feature reduction; combined with mip assignment for stats.
		for (u_poly = 0; u_poly < parpoly.uLen; ++u_poly)
		{
			if (parpoly[u_poly].bPrerasterized)
				continue;

			parpoly[u_poly].ReduceFeatures();

			//
			// Determine the mip level for all the polygons in the scene.
			// Note: must be done after projection.
			//
			if (pSettings->seterfState[erfMIPMAP])
				parpoly[u_poly].SetMipLevel(false);
			else
				parpoly[u_poly].iMipLevel = parpoly[u_poly].ptexTexture->iGetNumMipLevels()-1;
		}
		psFeature.Add(ctmr(), parpoly.uLen);

		// Select appropriate sorting method.
		switch (pSettings->esSortMethod)
		{
			case esPresortFrontToBack:
				// Presort list front to back.
				MakeSortedPointerList(parpoly, renc.rplhHeap, true);
				break;
			case esPresortBackToFront:
				// Presort list back to front.
				MakeSortedPointerList(parpoly, renc.rplhHeap, false);
				break;
			case esDepthSort:
				// Depth sort list back to front.
				DepthSortPolygons(renc.rplhHeap, renc.Camera);
				break;
			default:
				// Fill pointer array unsorted.
				MakePointerList(parpoly, renc.rplhHeap);
		}

		// Add an optional alpha overlay polygon.
		if (pScreenRender->bTargetMainScreen())
			Overlay.Add(renc.Camera, renc.rplhHeap);

		// Local copy of the array pointer.
		CPArray<CRenderPolygon*> paprpoly = renc.rplhHeap.parppolyPolygons();

		#if bVERIFY_DEPTHSORT_ORDER

			// Verify that the depth sorted order is correct.
			if (pSettings->esSortMethod == esDepthSort)
			{
				Assert(bVerifyOrder(paprpoly));
			}

		#endif // bVERIFY_DEPTHSORT_ORDER

		// Add sorting stats.
		proProfile.psPresort.Add(ctmr(), parpoly.uLen);

		//
		// Rasterize everything.
		//
		proProfile.psBeginFrame.Add(ctmr());

		//
		// Draw the polygon list.
		//
		if (!pSettings->bTargetCache && paprpoly.uLen > 0)
		{
			DumpPolylist(paprpoly);
			ValidatePolylist(paprpoly);
		}
		pScreenRender->DrawPolygons(paprpoly);
		proProfile.psDrawPolygon.Add(ctmr());
	}

	//**********************************************************************************************
	void CRenderer::UpdateSettings()
	{
		pScreenRender->UpdateSettings();
	}

	//******************************************************************************************
	void CRenderer::ExecuteScheduleForTerrain()
	{
		if (!pSettings->bExecuteScheduler)
			return; 
		
		CCycleTimer cmtr_terrain;

		// Execute scheduled terrain updates after forcing hardware to finish.
		srd3dRenderer.FlushBatch();

		//
		// Note that because terrain textures are placed on the schedule list in the poly iterator texture access
		// function, scheduled terrain texture rebuilds must be executed AFTER the current scene has been rasterised,
		// The render poly structure contains information (specifically: texture coordinates and render flags
		// settings) about the current terrain texture. Executing the scheduler for terrain texture rebuilds before
		// this render poly is rasterised would invalidate this information.
		//
		shcSchedulerTerrainTextures.Execute();
		
	#if (VER_TIMING_STATS)
		//
		// Time has been added to the terrain texture stat for the scheduled items.
		// Since the terrain texture stat is under psRenderShape and is subtracted
		// from it (epfSEPARATE) we should add the time for doing the terrain 
		// texture to psRenderShape.
		//
		proProfile.psRenderShape.Add(CScheduler::cyAccountedScheduleCycles);
	#endif

		psExecuteTerrain.Add(cmtr_terrain(), 1);
	}

	//******************************************************************************************
	void CRenderer::ExecuteScheduleForCaches()
	{
		//
		// To do:
		//		Move the render cache schedule code into this function.
		//
	}

	//******************************************************************************************
	void CRenderer::RasteriseZBufferTerrain(CRenderContext& renc)
	{
		CCycleTimer ctmr;

		// If the Z buffer is not active or hardware is not the rasterizing target, do nothing.
		if (!pScreenRender->bTargetHardware())
			return;

		// Get the current polygon array.
		CPArray<SRenderVertex>  parv    = renc.rplhHeap.parvVertices();
		CPArray<CRenderPolygon> parpoly = renc.rplhHeap.parpolyPolygons();

		// Project vertices.
		renc.Camera.ProjectVertices(parv);
		psProject.Add(ctmr(), parv.uLen);

		// Rasterize a batch of terrain polygons.
		RasterizeTerrainBatch(parpoly);

		// Add the polygons and vertices to their respective counters.
		renc.uNumPolysPrerasterized = parpoly.uLen;
		renc.uNumVertsProjected     = parv.uLen;
	}

	//**********************************************************************************************
	void CRenderer::RasteriseZBufferBatch(CRenderContext& renc)
	{
		CCycleTimer ctmr;
		uint u_poly;

		// Add particles.
		Particles.Add(renc.Camera, renc.rplhHeap);

		// Construct arrays which reference only the portions of the arrays built this function.
		CPArray<SRenderVertex>  parv    = renc.rplhHeap.parvVertices();
		CPArray<CRenderPolygon> parpoly = renc.rplhHeap.parpolyPolygons();

		parpoly.uLen    -= renc.uNumPolysPrerasterized;
		parv.atArray    += renc.uNumVertsProjected;
		parv.uLen       -= renc.uNumVertsProjected;
		parpoly.atArray += renc.uNumPolysPrerasterized;

		if (!parpoly.uLen)
		{
			if (gpaiSystem)
				gpaiSystem->ProcessPending();
			return;
		}

		// Project vertices.
		renc.Camera.ProjectVertices(parv);
		psProject.Add(ctmr(), parv.uLen);

		// Set the target device.
		srd3dRenderer.SetOutputFlag(true);
		pScreenRender->SetHardwareOut(true);
		psFeature.Add(ctmr(), 0);

		// Rasterize all caches in the list.
		RasterizeCacheBatch(parpoly);
		psBatchCaches.Add(ctmr(), parpoly.uLen);

		// Feature reduction; combined with mip assignment for stats.
		pScreenRender->SetD3DFlagForPolygons(parpoly, true);
		for (u_poly = 0; u_poly < parpoly.uLen; ++u_poly)
		{
			CRenderPolygon& rp = parpoly[u_poly];
			if (rp.bPrerasterized)
				continue;

			parpoly[u_poly].ReduceFeatures();
			parpoly[u_poly].SetMipLevel(true);
		}
		psFeature.Add(ctmr(), parpoly.uLen);

		// Upload textures to card if required.
		srd3dRenderer.LoadHardwareTextures(parpoly, true);
		psUploadTextures.Add(ctmr(), parpoly.uLen);

		// Set out the remaining polygons in a batch.
		RasterizeBatch(parpoly);
		psBatchOther.Add(ctmr(), parpoly.uLen);

		// Process AI here as there the CPU might stall here anyway.
		if (gpaiSystem)
			gpaiSystem->ProcessPending();

		//
		// Depthsort the remaining unrasterized polygons with software-only polyons.
		//
		parpoly = renc.rplhHeap.parpolyPolygons();
		RemovePrerasterized(parpoly);
		DepthSortPolygons(renc.rplhHeap, renc.Camera);
		proProfile.psPresort.Add(ctmr(), parpoly.uLen);

		//
		// Draw the depth-sorted polygons.
		//
		pScreenRender->BeginFrame();
		pScreenRender->DrawPolygons(renc.rplhHeap.parppolyPolygons());

		// Draw optional alpha overlay polygon.
		Overlay.Add(renc.Camera, renc.rplhHeap);

		srd3dRenderer.EndScene();
		proProfile.psDrawPolygon.Add(ctmr());
	}


//
// Global variables.
//

// The main renderer.
ptr<CRenderer> prenMain;
