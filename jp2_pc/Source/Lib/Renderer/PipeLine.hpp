/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		The rendering pipeline. The main entry point for the 3d renderer.
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/PipeLine.hpp                                             $
 * 
 * 66    8/25/98 11:42a Rvande
 * Clarified ambiguous access
 * 
 * 65    8/04/98 6:18p Pkeet
 * Added the 'RasteriseZBufferBatch' member function. Added data member to 'CRenderContext' to
 * track the number of prerasterized polygons rasterized so far to replace the clumsier tracking
 * method.
 * 
 * 64    8/01/98 11:08p Pkeet
 * Added the 'RasteriseZBufferTerrain' member function.
 * 
 * 63    7/23/98 6:24p Pkeet
 * Added support for texturing in hardware out of system memory.
 * 
 * 62    6/19/98 5:20p Pkeet
 * Fixed bug that caused cache fogging to not match uncached fogging.
 * 
 * 61    6/14/98 2:48p Pkeet
 * Added support for fogging render caches using hardware.
 * 
 * 60    5/23/98 5:37p Pkeet
 * Added member functions for executing scheduled operations.
 * 
 * 59    5/05/98 5:08p Pkeet
 * Added objects that always face the camera. Added an optimization that uses polygon
 * intersection in place of box intersection for simple meshes and objects that always face the
 * camera.
 * 
 * 58    4/09/98 5:55p Mlange
 * Can now optionally disable rendering of moving and static shapes.
 * 
 * 57    2/11/98 9:14p Pkeet
 * Added the 'bUseDistanceCulling' flag.
 * 
 * 56    1/23/96 3:34p Pkeet
 * Added the 'bExecuteScheduler' to the pipeline settings.
 * 
 * 55    97/11/20 3:41p Pkeet
 * Renders triggers based on a flag in render context.
 * 
 * 54    97/11/18 14:59 Speter
 * Fixed shadow partition bug, and sped it up a bit.
 * 
 * 53    97/11/05 22:32 Speter
 * Sped up, simplified RenderPartition() some.
 * 
 * 52    97/10/23 12:45p Pkeet
 * Added a shape to normalized camera space transform parameter for the partition render test
 * member functions.
 * 
 * 51    97/10/22 5:45p Pkeet
 * Added the 'b_execute_schedule' to a render scene function.
 * 
 * 50    97/10/10 2:23p Pkeet
 * Added the 'esfRenderPartitionTestBox' member function.
 * 
 * 49    97/10/10 10:51a Pkeet
 * Added the 'esfRenderPartitionTest' member function and moved code there from
 * 'RenderPartition.'
 * 
 * 48    97/10/06 4:52p Pkeet
 * Added occlusion to the 'RenderScene' and 'RenderPartition' interfaces.
 * 
 * 47    97/09/23 13:24 Speter
 * Hacked SSettings::operator= to avoid brain-dead compiler bug.
 * 
 * 46    9/03/97 6:42p Pkeet
 * Replaced the 'seteppPartitionProperties' flags with a bitset.
 * 
 * 45    8/28/97 4:10p Agrant
 * Source Safe Restored to Tuesday, August 26, 1997
 * 
 * 45    97/08/22 11:23 Speter
 * Replaced bAlignObjectsHorizontal feature with bTerrainHeightRelative, for proper shadowing of
 * objects above terrain.
 * 
 * 44    97/08/18 10:40 Speter
 * Added support for selective rendering based on EPartitionProperties.
 * 
 * 43    97/08/17 19:28 Speter
 * Added bAlignObjectsHorizontal feature, and pr3ObjectPresence() function, for shadows.
 * 
 * 42    97/08/08 15:32 Speter
 * Added CRenderer::SSettings::bBackfaceCull setting.
 * 
 * 41    97/07/30 11:46a Pkeet
 * Added the 'CRemapPosition' class to handle fogging.
 * 
 * 40    97/07/28 11:18a Pkeet
 * Added the 'bTargetCache' member variable to the render settings structure.
 * 
 * 39    97/07/07 14:12 Speter
 * Consolidated various render objects in CRenderContext, for use in CShape::Render().  Added a
 * version of RenderScene() taking a top-level partition, with integrated query, rather than a
 * partition list.
 * Also added one which takes a pre-made CLightList rather than a list of light instances.
 * 
 * 38    97/06/14 0:10 Speter
 * Added settings for detail reduction; moved default constructor to .cpp file.
 * 
 * 37    97/05/26 3:47p Pkeet
 * Made depthsorting the default state of the renderer.
 * 
 * 36    97-05-08 13:13 Speter
 * Removed RenderShape() member function, as it now renders through each shape's Render().
 * 
 * 35    4/30/97 9:19p Rwyatt
 * Moved esfView member variable from CPartition and put it into the list that the partition
 * functions return. These functions used to return a list of CPartition* now they return a list
 * of structures that contain a CPartition* and esfSideOf.
 * 
 * 34    97-04-14 20:29 Speter
 * Added esf_view parameter to RenderShape.  Removed bClip member, replaced with bObjectReject
 * and bObjectAccept (currently always on).
 * 
 * 
 * 33    97/04/05 2:50p Pkeet
 * Made changes to accomodate depth sorting.
 * 
 * 32    97/04/04 4:32p Pkeet
 * Fixed heap bug by removing the static CPipelineHeap object and replacing it with references
 * to locally created objects.
 * 
 * 31    97-04-03 19:17 Speter
 * Removed bUpdateWorld and associated functions; now handled by CEntityLight.
 * 
 * 30    97-03-31 22:05 Speter
 * Added MovedLight() processing function.
 * 
 * 29    97-03-28 16:14 Speter
 * Created CRenderer from pipeline code, containing pScreenRender.
 * Added CRenderer::SSettings, derived from CScreenRender::SSettings, and containing
 * non-screen-specific settings.
 * Moved bClip from SRenderSettings to CRenderer::Settings
 * 
 * 28    97/03/24 15:06 Speter
 * Replaced camera CInstance with CCamera.
 * 
 * 27    97/03/21 2:00p Pkeet
 * Changed to accomodate recursive image caching.
 * 
 * 26    97/01/26 19:42 Speter
 * Changed list argument to const&, to prevent copying the whole list.
 * 
 * 25    97/01/20 11:35 Speter
 * Added b_try_cache parameter.  Changed list parameter to const&.
 * 
 * 24    96/12/09 16:20 Speter
 * Updated for new CProfile members, removed StatDB stats, as well as iSort and iNumTriangles.
 * 
 * 
 * 23    12/09/96 1:45p Mlange
 * Updated for changes to the CCamera interface. RenderScene() now takes a CInstance for the
 * camera. Made some function parameters const references instead of passing them by value.
 * 
 * 22    12/03/96 6:51p Pkeet
 * Added triangle counting for speed tests.
 * 
 * 21    12/03/96 5:10p Pkeet
 * Added global variables for presorting.
 * 
 * 20    11/27/96 12:33p Mlange
 * Removed the SRenderObject structure declaration. The RenderScene function now takes lists of
 * CInstance types for the objects and lights in the scene.
 * 
 * 19    11/23/96 5:24p Mlange
 * Changed interface to RenderScene(). Removed redundant #include's.
 * 
 * 18    11/21/96 12:03p Mlange
 * Removed some redundant includes.
 * 
 * 17    11/11/96 9:44p Agrant
 * The Big Change!
 * Object hierarchy now based on CInstance instead of CEntity/CPresence.
 * CEntityShape/CEntityPhysics removed.
 * 
 * 16    10/31/96 5:55p Pkeet
 * Changed 'prscWorld' to 'wWorld' and references to 'TPrsContainer' to 'CWorld.'
 * 
 * 15    10/28/96 7:12p Pkeet
 * Removed the light list as a parameter of the Render function; the light list is now built
 * internally from the world database.
 * 
 * 14    10/24/96 6:30p Pkeet
 * Changed the 'CPresence& prs_scene' parameter to 'TPrsContainer& petpc_scene' in the
 * 'RenderScene' function.
 * 
 * 13    96/10/22 11:27 Speter
 * Changed CPresenceCamera to CCamera, prscam prefix to cam.
 * 
 * 12    96/10/22 11:14 Speter
 * Changed CEntity to CPresence in appropriate contexts.
 * 
 * 11    96/10/04 17:59 Speter
 * Updated the To Do list.
 * 
 * 10    96/10/04 17:55 Speter
 * Removed old includes.
 * 
 * 9     96/09/05 11:45 Speter
 * Removed sererfRendering global var.
 * 
 * 8     96/08/06 18:19 Speter
 * Changed argument order of RenderScene.  And now takes top-level CEntity to render.
 * 
 * 7     96/07/22 15:39 Speter
 * Changes for new rendering object format:
 * Added seterfRendering global var.
 * Added CLight param to RenderScene.
 * 
 * 6     7/18/96 6:59p Mlange
 * Updated for name changes of CVertex to CPoint. Updated todo list.
 * 
 * 5     7/17/96 1:26p Mlange
 * Removed external declarations for the renderer's heaps. Moved definitions of the constants
 * for the sizes of the heaps to the .cpp file.
 * 
 * 4     7/08/96 6:00p Mlange
 * Updated the todo list.
 * 
 * 3     6/26/96 12:36p Mlange
 * Temporary version!!
 * Updated the todo list. Added constants and extern declarations for the renderer's heaps.
 * 
 * 2     96/06/18 10:07a Mlange
 * Operational.
 * 
 * 1     11-06-96 1:42p Mlange
 * Initial (empty) definition.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_RENDERER_PIPELINE_HPP
#define HEADER_LIB_RENDERER_PIPELINE_HPP

#include "ScreenRender.hpp"
#include "Lib/EntityDBase/Instance.hpp"

#include <list>

//********************************************************************************************
//
// Definitions for CRenderer
//

// Sorting method.
enum ESort { esDepthSort, esPresortFrontToBack, esPresortBackToFront, esNone };

// Type enumerating a bitset of partition attributes.
enum EPartitionProperties { eppCACHEABLE, eppCASTSHADOW };

class CRenderer;
class CScreenRender;
class CCamera;
class CLightList;
class CShape;
class CPipelineHeap;
class COcclude;


//**********************************************************************************************
//
class CRenderContext
//
// Prefix: renc
//
// A structure containing info needed to render a particular scene, including lights, camera, 
// and heap.
//
//**************************************
{
public:

	const CRenderer& Renderer;					// The renderer for this rendering.
	CPipelineHeap&   rplhHeap;					// The heap for this rendering.
	const CCamera&   Camera;					// The camera.
	CTransform3<>    tf3ToNormalisedCamera;		// The world-to-normalised-camera transform.
	CLightList&      rLightList;				// The light list.
	bool             bTargetHardware;			// Flag for Direct3D rasterization.
	uint             uNumPolysPrerasterized;	// Number of polygons prerasterized using Direct3D.
	uint             uNumVertsProjected;		// Number of vertices projected already.

	// Static globals.
	static bool      bRenderTriggers;			// Renders triggers if set.

	// Constructor.
	CRenderContext
	(
		const CRenderer&	ren,
		CPipelineHeap&		rplh,
		const CCamera&		cam,
		CLightList&			rltl
	);
};

//********************************************************************************************
//
class CRenderer
//
// Prefix: ren
//
// An object used for 3D rendering.
//
//**************************************
{
public:

	//******************************************************************************************
	//
	struct SSettings: public CScreenRender::SSettings
	//
	// Prefix: renset
	//
	// Settings for rendering behaviour.  Incorporates screen rendering (rasterising) settings,
	// as well as 3D-particular settings.
	//
	//**************************************
	{

		//**************************************************************************************
		//
		class CRemapPosition
		//
		// Class remaps between normalized camera z values.
		//
		// Prefix: rem
		//
		// Notes:
		//		The rescaling uses an additional multiple of 0.9 because radial fogging is not
		//		being used, and this value represents the mean value between the minimum and
		//		maximum camera 'y' value for a camera with a view angle of 75 degrees, ie.,
		//
		//			(1.0 + cos(75 degrees)) ~
		//			----------------------- = 0.9
		//			            2
		//
		//**************************************
		{
			float fRescale;
		public:

			//**********************************************************************************
			//
			// Constructors.
			//

			// Default constructor.
			CRemapPosition()
				: fRescale(0.9f)
			{
			}

			// Constructor with initial values.
			CRemapPosition
			(
				float f_maincam_farz,	// Far clipping plane of the original camera.
				float f_newcam_farz		// Far clipping plane of the target camera.
			)
				: fRescale(f_newcam_farz / f_maincam_farz * 0.9f)
			{
			}

			//**********************************************************************************
			//
			// Member functions.
			//

			//**********************************************************************************
			//
			float fRemapToOriginal
			(
				float f
			) const
			//
			// Returns a value converted from the target camera space to the original camera
			// space.
			//
			//**************************************
			{
				return f * fRescale;
			}
		};

		CSet<EPartitionProperties> seteppRequired,	// Rendered objects must have all these flags.
								   seteppForbidden;	// Rendered objects must have none of these flags.
		bool bObjectReject, bObjectAccept;			// Implement trivial object rejection/acceptance.
		bool bRenderCache;							// Try to cache images of objects.
		bool bDetailReduce;							// Select lower-detail objects when appropriate.
		float fDetailReduceFactor;					// Multiplier for shape's preferred screen area.
		bool bShadow;								// Calculate shadows.
		bool bTargetCache;							// Indicates that the target is a cache.
		bool bBackfaceCull;							// Whether to backface-cull polygons;
													// if false, assumes they are always facing.
		bool bTerrainHeightRelative;				// Offset all objects' heights by terrain height.
													// (useful for rendering shadows onto terrain texture).
		bool bRenderStaticObjects;					// Will render static (non-moving) objects if set.
		bool bRenderMovingObjects;					// Will render moving objects if set.
		bool bExecuteScheduler;						// Determines if scheduled operations will be
													// executed.
		CRemapPosition remRemapPosition;			// Object to remap camera z coordinates for
													// fogging.
		bool bUseDistanceCulling;					// Uses the distance culling method.
		ESort esSortMethod;
		bool bHardwareCacheFog;						// Use hardware to fog render caches.

		//**************************************************************************************
		SSettings();

		//**************************************************************************************
		SSettings& operator =(const SSettings& renset)
		// Prevent a bug in imbecilic VC 4.2, which implemented the default copy operator by
		// calling itself instead of the CScreenRender::SSettings copy operator.
		{
			memcpy(this, &renset, sizeof(*this));
			return *this;
		}
	};

	ptr<SSettings> pSettings;						// The 3D render settings to use
													// (stored somewhere else, like a shell).
	aptr<CScreenRender>	pScreenRender;				// Owned screen renderer (rasteriser) to use
													// for rendering.

public:

	//******************************************************************************************
	CRenderer
	(
		CScreenRender* psr,							// Screen rendering context.
		SSettings* pset								// 3D settings to apply.
	);

	//******************************************************************************************
	~CRenderer();

	//******************************************************************************************
	//
	void RenderScene
	(
		const CCamera& cam,							// Camera for this scene.
		const std::list<CInstance*>& listins_lights,		// A list of lights in the scene.
		CPartition* ppart_scene,					// The world partition to render.
		const CPArray<COcclude*>& rpapoc,				// Array of occluding objects.
		ESideOf esf_view = esfINTERSECT,			// The presumed relation of the scene to the
													// camera volume.  If esfINTERSECT, queries
													// will be done on the partition to find those
													// intersecting the camera.  If esfINSIDE,
													// containment will be assumed.
		CPartition* ppart_terrain = 0				// The separate terrain object, if desired.
	);
	//
	// Renders all objects in the given partition, and the terrain if specified, to the screen of 
	// the render context.  Performs scene setup, calls RenderPartition recursively to create a 
	// polygon list, and performs final sorting and rasterisation.
	//
	// Notes:
	//		This is the main entry point for the renderer.
	//
	//		The camera's properties must be up to date.
	//
	//**********************************

	//******************************************************************************************
	//
	void RenderScene
	(
		const CCamera& cam,					// Camera for this scene.
		CLightList& rltl_lights,			// The light list for the scene.
		CPartition* ppart_scene,			// The world partition to render.
		const CPArray<COcclude*>& rpapoc,	// Array of occluding objects.
		ESideOf esf_view = esfINTERSECT,	// The presumed relation of the scene to the
											// camera volume.  If esfINTERSECT, queries
											// will be done on the partition to find those
											// intersecting the camera.  If esfINSIDE,
											// containment will be assumed.
		CPartition* ppart_terrain = 0		// The separate terrain object, if desired.
	);
	//
	// Similar to above function, but uses a CLightList rather than a list<CInstance*>.
	//
	// Cross-references:
	//		Called by the above RenderScene(), and by the render cache creation code.
	//
	//**********************************

	//**********************************************************************************************
	//
	void RenderScene
	(
		const CCamera& cam, 
		const TPartitionList& listpart_shapes
	);
	//
	//**********************************

	//******************************************************************************************
	//
	void UpdateSettings();
	//
	// Perform any update needed based on new current settings.
	//
	// Cross-references:
	//		This function calls pScreenRender->UpdateSettings() as well.
	//
	//**************************************

	//******************************************************************************************
	//
	void ExecuteScheduleForTerrain
	(
	);
	//
	// Executes scheduled terrain rebuilds.
	//
	//**************************************

	//******************************************************************************************
	//
	void ExecuteScheduleForCaches
	(
	);
	//
	// Executes scheduled render cache rebuilds.
	//
	//**************************************

	//******************************************************************************************
	//
	bool bTargetScreen
	(
	) const
	//
	// Returns 'true' if the render target is the screen.
	//
	//**************************************
	{
		Assert((const CRenderer::SSettings*)pSettings != NULL);
		return !pSettings->bTerrainHeightRelative && !pSettings->bTargetCache;
	}

protected:

	//**********************************************************************************************
	//
	void RenderPartition
	(
		CRenderContext&			renc,					// General rendering info for scene.
		const CPresence3<>&		pr3_cam_inv,			// Camera's inverse presence (for efficiency).
		CPartition*             ppart,					// Partition to render.
		CPArray<COcclude*>		papoc,					// Array of occlusion objects.
		ESideOf					esf_view = esfINTERSECT	// Partition's relation to the camera volume.
	);
	//
	// Render all objects in the given partition, adding their transformed and lit polygons to a list, 
	// creating and utilising render caches as needed.
	//
	// Notes:
	//		This is called recursively by RenderScene(), after all scene setup has been performed,
	//		and before final rasterisation.
	//
	//**********************************

	//**********************************************************************************************
	//
	void AdjustPresence
	(
		CPresence3<>& pr3,				// The presence of a partition.
		const CPartition* ppart,		// The partition producing the presence.
		const CCamera& cam				// Camera currently rendering.
	) const;
	//
	// Alters an object's presence as necessary for particular rendering contexts,
	// e.g. shadowing.
	//
	//**********************************

	//**********************************************************************************************
	//
	void RasteriseScene
	(
		CRenderContext& renc	// The polygon list.
	);
	//
	// Takes the pre-constructed polygon list and rasterises it.
	//
	//**********************************

	//**********************************************************************************************
	//
	void RasteriseZBufferTerrain
	(
		CRenderContext& renc	// The polygon list.
	);
	//
	// Rasterises the current list of polygons using the Z buffer in hardware.
	//
	// Notes:
	//		This function assumes that all of the polygons in the list to date are terrain polygons.
	//
	//**********************************

	//**********************************************************************************************
	//
	void RasteriseZBufferBatch
	(
		CRenderContext& renc	// The polygon list.
	);
	//
	// Rasterises the current list of polygons not already rasterized using the Z buffer in hardware.
	//
	//**********************************
};

extern ptr<CRenderer> prenMain;

#endif

