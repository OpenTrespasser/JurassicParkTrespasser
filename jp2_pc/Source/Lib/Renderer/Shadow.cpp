/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Implementation of Shadow.hpp.
 *
 * To do:
 *		Terrain: Do more accurate object leveling when needed, accounting for any height above terrain.
 *		Similarly, scale object size based on projected size on terrain.
 *		Merge object and terrain shadow systems.
 *
 *		Incorporate anti-aliasing into shadowing query (need lighting region size).
 *		Change general camera interface to overload transform/clip/project.  Try to combine them.
 *		Integrate Z-buffer scale with camera projection, so one knows about the other.
 *			(Use ZBufferSurface?).
 *		Generalise to point lights when needed (can't use up-facing camera trick).
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/Shadow.cpp                                               $
 * 
 * 41    9/15/98 9:14p Pkeet
 * Turned shadow occlusion off.
 * 
 * 40    9/09/98 3:09p Pkeet
 * Added a parameter to make occlusion more effecient for shadows.
 * 
 * 39    9/09/98 1:02p Pkeet
 * Added occlusion.
 * 
 * 38    9/05/98 8:04p Agrant
 * removed warning about long symbols
 * 
 * 37    98/08/28 12:08 Speter
 * Moved far clipping plane back out, as shadows were not working when partitions were built,
 * for some mysterious reason.
 * 
 * 36    98/08/13 17:03 Speter
 * Clip shadows to terrain floor.
 * 
 * 35    98.07.28 2:21p Mmouni
 * Now recognized bump maps textures by using the erfBUMP flag.
 * 
 * 34    5/01/98 8:06p Mlange
 * Now enables trapezoid flag for shadows.
 * 
 * 33    4/14/98 5:36p Mlange
 * Now sets shadow render settings mipmap flag based on global mipmap flag.
 * 
 * 32    4/13/98 4:44p Mlange
 * Rendering of moving and/or static shadows is now optional.
 * 
 * 31    4/09/98 12:05p Mlange
 * Minor optimisations.
 * 
 * 30    3/13/98 5:46p Mlange
 * Reduced include bloat.
 * 
 * 29    3/10/98 1:20p Pkeet
 * Added include to "LocalArray.hpp."
 * 
 * 28    2/11/98 9:15p Pkeet
 * Sets a default value for the 'bUseDistanceCulling' flag.
 * 
 * 27    98/02/04 15:24 Speter
 * Replaced fGetRoughScreenArea() with fArea.
 * 
 * 26    98.01.22 3:11p Mmouni
 * Now correctly deals with 16-bit bump-maps.
 * 
 **********************************************************************************************/

#include "Common.hpp"
#include "Shadow.hpp"

#include <float.h>
#include "Primitives/DrawTriangle.hpp"
#include "Camera.hpp"
#include "ScreenRenderShadow.hpp"
#include "Pipeline.hpp"
#include "Lib/EntityDBase/WorldDBase.hpp"
#include "Lib/Math/FloatDef.hpp"
#include "Lib/Std/LocalArray.hpp"
#include "Lib/Renderer/Occlude.hpp"

#pragma warning(disable: 4786)

const TReal rCAMERA_DIST = 1000;			// Arbitrary large value to make camera almost parallel.
const float fMIN_PIXELS_TRANSPARENT = 10;

// Switch for using occlusion to generate shadows.
#define bUSE_SHADOW_OCCLUSION (0)


//******************************************************************************************
//
class CScreenRenderShadowLight : public CScreenRender
//
// A screen renderer that shadows the light values of a composite texture map.
//
//**************************************
{
public:

	//******************************************************************************************
	CScreenRenderShadowLight(SSettings* pscrenset, rptr<CRaster> pras_screen, int i_light_level)
		: CScreenRender(pscrenset, pras_screen)
	{
		Assert(pras_screen->iPixelBytes() == 2);
		u1ShadowIntensity = i_light_level;
	}

	//******************************************************************************************
	//
	// Overrides.
	//

	//******************************************************************************************
	virtual void BeginFrame()
	{
		prasScreen->Lock();
	}

	//******************************************************************************************
	virtual void EndFrame()
	{
		prasScreen->Unlock();
	}

	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	void DrawPolygon(CRenderPolygon& rp)
	{
		CCycleTimer ctmr;

		Assert(rp.fArea >= 0 && fMIN_PIXELS_TRANSPARENT >= 0);

		// Invoke the CDrawPolygon code, using the special light shadowing primitive.
		if (rp.seterfFace[erfTRANSPARENT] && CIntFloat(rp.fArea).i4Int > CIntFloat(fMIN_PIXELS_TRANSPARENT).i4Int)
		{
			// Map the texture, using transparency to shadow.
			Assert(rp.ptexTexture);
			Assert(rp.ptexTexture->prasGetTexture());

			// Choose a bumpmap or an 8 bit texture map.
			if (rp.seterfFace[erfBUMP])
			{
				CDrawPolygon< TShadowTrans32 >(prasScreen, rp);
			}
			else if (rp.ptexTexture->prasGetTexture()->iPixelBits == 8)
			{
				CDrawPolygon< TShadowTrans8 >(prasScreen, rp);
			}

			//psShadowPixelsTrans.Add(ctmr(), 1);
		}
		else
		{
			// Solid shade, ignoring texture.
			CDrawPolygon< TShadow >(prasScreen, rp);
			//psShadowPixelsSolid.Add(ctmr(), 1);
		}
	}
};

//******************************************************************************************
struct SRenderSettingsShadow: public CRenderer::SSettings
// Prefix: rensetshad
//
// Customise settings for shadow lighting.  
//
{
	SRenderSettingsShadow()
	{
		seterfState = set0;

		// Disable unneeded features.
		bShadow = false;
		bRenderCache = false;
		esSortMethod = esNone;

		// No clear.
		bClearBackground = false;

		// Disable trivial acceptance for now, as camera query seems to be erroneously
		// returning objects as INSIDE.
		bObjectAccept = false;

		// Flag needed for correct shadowing.
		bTerrainHeightRelative = true;

		// Indicate that scheduled operations should not be executed.
		bExecuteScheduler = false;

		seteppRequired = Set(eppCASTSHADOW);

		// For now, do not using the distance culling method for shadows.
		bUseDistanceCulling = false;

		// Copy global flags.
		seterfState[erfMIPMAP]      = prenMain->pSettings->seterfState[erfMIPMAP];
		seterfState[erfTRAPEZOIDS]  = prenMain->pSettings->seterfState[erfTRAPEZOIDS];
		seterfState[erfTRANSPARENT] = prenMain->pSettings->seterfState[erfTRANSPARENT];
	}
};

//**********************************************************************************************
//
// Global functions.
//

	//******************************************************************************************
	void ShadowLighting
	(
		rptr<CRaster> pras_composite,
		const CRectangle<>& rc_world,
		const CDir3<>& d3_light,
		int i_light_level,
		bool b_moving_shadows,
		bool b_static_shadows
	)
	{
		CCycleTimer ctmr;

		if (d3_light.tZ >= 0)
			// This bloody light points sideways or upwards.  
			// That makes our horizontal terrain shadowing very hard.  I'm outy.
			return;

		if (rc_world.tWidth() <= 0 || rc_world.tHeight() <= 0)
			return;

		TReal r_shear_x = -d3_light.tX / d3_light.tZ;
		TReal r_shear_y = -d3_light.tY / d3_light.tZ;

		// Move the camera behind the centre of the view volume.
		CVector3<> v3_campos = rc_world.v2Mid();
		v3_campos.tZ = rCAMERA_DIST;
		v3_campos.tX -= rCAMERA_DIST * r_shear_x;
		v3_campos.tY -= rCAMERA_DIST * r_shear_y;

		// Create the properties for a distant perspective shadow camera.
		CCamera::SProperties camprop;

		camprop.bPerspective       = true;
		camprop.rViewWidth         = rc_world.tWidth() * 0.5 / rCAMERA_DIST;
		camprop.fAspectRatio       = rc_world.tWidth() / rc_world.tHeight();
		camprop.rNearClipPlaneDist = 1.0;
		camprop.rFarClipPlaneDist  = v3_campos.tZ * 2.0;
		camprop.vpViewport.SetVirtualOrigin(r_shear_x, r_shear_y);

		// Set the viewport screen width to correspond to the raster size.
		camprop.vpViewport.SetSize(pras_composite->iWidth, pras_composite->iHeight);

		// Point camera straight down.
		CPlacement3<> p3_camera(CRotate3<>(d3YAxis, -d3ZAxis), v3_campos);

		// Create the camera.
		CCamera cam(p3_camera, camprop);

		//
		// Construct a shadow screen renderer and renderer with this raster (and no Z buffer).
		//
		SRenderSettingsShadow rnsetshad;

		rnsetshad.bRenderMovingObjects = b_moving_shadows;
		rnsetshad.bRenderStaticObjects = b_static_shadows;

		ptr<CScreenRender> pscren = new CScreenRenderShadowLight(&rnsetshad, pras_composite, i_light_level);
		CRenderer ren(pscren, &rnsetshad);

		//psShadowSetup.Add(ctmr(), 1);

	#if bUSE_SHADOW_OCCLUSION

		//
		// Build an occlusion list for objects within the shadow camera volume.
		//

		TOccludeList oclist;	// List of occluding polygons.

		// Get a list from the world database.
		GetOccludePolygons
		(
			wWorld.ppartPartitionList(),	// Root node to search for occlusion objects.
			~cam.pr3Presence(),				// Inverse camera presence.
			cam.pbvBoundingVol(),			// Camera bounding volume.
			oclist							// Linked list of visible occluding objects.
		);
		
		// Create an array of occluding polygon objects.
		CLArray(COcclude*, papoc, oclist.size());

		// Copy the list to the array.
		CopyOccludePolygons(papoc, cam, oclist, false, false);

	#else

		// Construct an empty occlusion object.
		CLArray(COcclude*, papoc, 0);

	#endif // bUSE_SHADOW_OCCLUSION

		// Render all world objects with this camera and renderer.
		//pscren->BeginFrame();
		ren.RenderScene(cam, list<CInstance*>(), wWorld.ppartPartitionList(), papoc);
		pscren->EndFrame();
	}

//**********************************************************************************************
//
// class CShadowBuffer implementation.
//


	//
	// The shadow buffer is rendered with a parallel camera, and is thus formatted as a Z buffer
	// (not an inverse Z buffer).  The camera is placed below the world in order to render down-
	// facing polygons only.  The values increase from the near camera plane to the far plane.  
	// Thus, higher values are nearer to the light.
	//

	// Fudge factor for shadow plane sizes, to ensure it totally encloses contents.
	const float fVOLUME_FUDGE					= 1.1;

	// Fudge factor for shadow comparisons, to prevent self-shadowing.
	const int iSHADOW_TOLERANCE					= 4 << sizeof(TShadowPixel);

	//******************************************************************************************
	CShadowBuffer::CShadowBuffer(int i_dim, const CBoundVol& bv_world,
		const CPlacement3<>& p3_light)
		: CRasterMemT<TShadowPixel>(i_dim, i_dim)
	{
		// Find the shadow plane world size we need.
		CVector3<> v3_centre(0, 0, 0);
		TReal r_radius = bv_world.fMaxExtent() * fVOLUME_FUDGE;

		// Create the parallel shadow camera properties.
		CCamera::SProperties camprop;

		camprop.bPerspective = false;
		camprop.rViewWidth = r_radius;
		camprop.rNearClipPlaneDist = 0;
		camprop.rFarClipPlaneDist  = 2.0 * r_radius;

		// Set the viewport screen width to correspond to the raster size.
		camprop.vpViewport.SetSize(i_dim, i_dim);

		// Move the camera away from the centre of the volume by the world radius,
		// in the same direction as the light direction, so it's below the world.
		CDir3<> d3_light = d3ZAxis * p3_light;
		CVector3<> v3_cam_pos = v3_centre + d3_light * r_radius;

		// Point the camera (its Y axis) in the opposite direction as the light.
		CPlacement3<> p3_camera(CRotate3<>(d3YAxis, -d3_light), v3_cam_pos);

		// Create a camera instance.
		pCamera = new CCamera(p3_camera, camprop);
	}

	//******************************************************************************************
	void CShadowBuffer::Render(CPartition* ppart)
	{
		//
		// Create our own render settings.
		//
		CRenderer::SSettings renset = *prenMain->pSettings;

		// Enable only a single feature.
		renset.seterfState = Set(erfTRANSPARENT);

		// Prevent recursive shadowing.
		renset.bShadow = false;
		renset.bRenderCache = false;

		// Construct a shadow screen renderer with this raster (and no Z buffer).
		ptr<CScreenRender>	psr_context = pscrenShadow(&renset, rptr_cast(CRaster, rptr_this(this)));

		CRenderer ren(psr_context, &renset);

		psr_context->BeginFrame();

		// Clear to the minimum value (furthest from light).
		Clear(0);

		// Construct an empty occlusion object.
		CLArray(COcclude*, paoc, 0);

		// Render with our camera, and an empty light list.
		ren.RenderScene(*pCamera, list<CInstance*>(), ppart, paoc);
		psr_context->EndFrame();
	}

	//******************************************************************************************
	void CShadowBuffer::SetViewingContext(const CPresence3<>& pr3)
	{
		tf3ObjectCamera = ~pr3 * pCamera->tf3ToNormalisedCamera();
	}

	//******************************************************************************************
	bool CShadowBuffer::bWithinBuffer(const CVector3<>& v3_object)
	{
		CVector3<> v3_shadow = pCamera->ProjectPoint(v3_object * tf3ObjectCamera);

		return v3_shadow.tX >= 0 && v3_shadow.tX < float(iWidth) &&
			   v3_shadow.tY >= 0 && v3_shadow.tY < float(iHeight) &&
			   v3_shadow.tZ >= 0 && v3_shadow.tZ < 1;
	}

	//******************************************************************************************
	bool CShadowBuffer::bShadowed(const CVector3<>& v3_object) const
	{
		// Transform the point to our shadow buffer space.
		CVector3<> v3_shadow = pCamera->ProjectPoint(v3_object * tf3ObjectCamera);

		// Make sure it's in range.
		if (!(v3_shadow.tX >= 0 && v3_shadow.tX < float(iWidth) &&
			  v3_shadow.tY >= 0 && v3_shadow.tY < float(iHeight) &&
			  v3_shadow.tZ >= 0 && v3_shadow.tZ < 1))
			  return false;

		// X and Y are now pixel values.  Index them into the raster.
		// Z is the height value, ranging from 0 (far) to shpixMAX (near).  
		// Compare it with the value in the raster.
		int i_x = iPosFloatCast(v3_shadow.tX);
		int i_y = iPosFloatCast(v3_shadow.tY);
		int i_z = iPosFloatCast(v3_shadow.tZ * fSHADOW_Z_MULTIPLIER) + iSHADOW_TOLERANCE;

		return i_z < tPix(i_x, i_y);
	}
