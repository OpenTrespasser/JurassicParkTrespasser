/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Implementation of Camera.hpp.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/Camera.cpp                                               $
 * 
 * 97    10/05/98 5:46a Pkeet
 * Tweaked z buffer.
 * 
 * 96    10/01/98 12:52a Asouth
 * Fixed mis-femmage
 * 
 * 95    8/27/98 1:49p Asouth
 * had to specify conversion for delete operator
 * 
 * 94    98.08.19 6:01p Mmouni
 * Moved all quality setting values and multipliers into a single structure.
 * 
 * 93    8/15/98 10:07p Pkeet
 * Uncommented 'Unproject Point.'
 * 
 * 92    8/11/98 2:16p Agrant
 * changed include to allow compilation
 * 
 * 91    8/02/98 4:49p Pkeet
 * Based the 'fInvZAdjust' value on a Direct3D constant.
 * 
 * 90    8/02/98 3:38p Pkeet
 * Added an important comment.
 * 
 * 89    98.06.25 8:46p Mmouni
 * Added stuff to support render quality setting.
 * 
 * 88    5/25/98 12:10a Pkeet
 * Added a parent camera pointer and access member functions to it.
 * 
 * 87    98.05.06 7:00p Mmouni
 * Changed default FOV to 75.
 * 
 * 86    4/22/98 4:52p Pkeet
 * Changed partition flag access to use the new access function.
 * 
 * 85    3/23/98 2:54p Agrant
 * Only load camera properties that might be changed during game play.
 * 
 * 84    98/03/02 21:45 Speter
 * Changed pcSave to explicitly save presence, since CInstance thinks the camera doesn't move.
 * Ditto pcLoad.
 * 
 * 83    98/02/10 13:15 Speter
 * Replaced changed CSArray with CPArray in WorldExtents.
 * 
 * 82    98/01/12 20:00 Speter
 * Set bCastShadow flag in constructor.
 * 
 * 81    1/09/98 12:14p Pkeet
 * The FPU now truncates for perspective projection.
 * 
 * 80    1/08/98 5:05p Pkeet
 * Now the viewport is made slightly smaller to prevent rounding errors from causing hardware
 * accelerators from cracking due to vertices that are slightly outside of the view area. This
 * has not been a problem previously with the software renderer.
 * 
 * 79    12/17/97 5:55p Gfavor
 * Added multiply by fInvZAdjust to ProjectPointsPerspective and back into ProjectVertices.
 * Completed conversion of all of ProjectVertices to 3DX.
 * 
 * 78    12/17/97 4:18p Gfavor
 * Converted perspective projection part of ProjectVertices to 3DX.  Removed unnecessary
 * multiply from both C and 3DX versions of this calc.
 * 
 * 77    97/11/15 10:52p Pkeet
 * Added the 'fViewWidthRatio' data member to the camera properties.
 * 
 * 76    11/10/97 8:49p Agrant
 * Save/Load camera first pass complete.
 * 
 * 75    11/10/97 6:08p Agrant
 * Placeholder save/load, intermediate check-in
 * 
 * 74    10/28/97 7:03p Mlange
 * Fixed bug in CCamera::WorldExtents(). It did not take the presence of the camera into account
 * when transforming the points to world space.
 * 
 * 73    10/27/97 5:27p Mlange
 * Added CCamera::vsBoundingSphere() and CCamera::WorldExtents().
 * 
 **********************************************************************************************/

#include "common.hpp"
#include "Lib/W95/WinInclude.hpp"
#include "Camera.hpp"

#include "Lib/Renderer/GeomTypes.hpp"
#include "Lib/Math/FastTrig.hpp"
#include "Lib/Math/FastInverse.hpp"
#include "Lib/EntityDBase/MovementPrediction.hpp"
#include "Lib/EntityDBase/WorldDBase.hpp"
#include "AsmSupport.hpp"
#include "Lib/Renderer/ScreenRenderAuxD3DUtilities.hpp"
#include "Lib/EntityDBase/QualitySettings.hpp"


//**********************************************************************************************
//
// CCamera implementation.
//

	//**********************************************************************************************
	//
	// SProperties implementation.
	//

		const float fMONITOR_ASPECT = 4.0 / 3.0;

		//**************************************************************************************
		CCamera::SProperties::SProperties() :
			bPerspective(true), bClipNearFar(true),
			fZoomFactor(1.0f), rNearClipPlaneDist(0.025f), rFarClipPlaneDist(1000.0f),
			rDesiredFarClipPlaneDist(1000.0f), fAspectRatio(fMONITOR_ASPECT)
		{
			SetAngleOfView(dDegreesToRadians(75.0));

			//
			// Set the viewport virtual coordinates to correspond with virtual screen space:
			// -1 to +1, with Y axis going up..
			// Note: we do not need to shrink this viewport by any tolerance, because the
			// rasteriser rounds screen coords to the nearest integer, thus compensating for
			// small inaccuracies.
			//
			vpViewport.SetVirtualSize(2, -2);
			vpViewport.SetVirtualOrigin(0, 0);

			// The points are also translated by an additional .5 in the x and y directions so
			// that the drawing primitives can simply truncate (instead of round) to the centre of
			// an integer pixel.
			vpViewport.SetPosition(0.5, 0.5);
		}

		//**************************************************************************************
		CAngle CCamera::SProperties::angGetAngleOfView() const
		{
			if (!bPerspective)
				return 0;
			else
			{
				// Determine the angle from the ratio of width to depth;
				float f_angle = 2.0 * atan(rViewWidth);
				return CAngle(f_angle);
			}
		}
			
		//**************************************************************************************
		void CCamera::SProperties::SetAngleOfView(CAngle ang)
		{
			if (ang == CAngle(0))
				// This corresponds to a parallel camera.
				bPerspective = false;
			else
			{
				// Alter the width to correspond to the angle's tangent.
				bPerspective = true;
				rViewWidth = (ang / 2).fTan();
				Assert(_finite(rViewWidth));
			}
		}

		//**************************************************************************************
		//
		void CCamera::SProperties::SetFarClipFromDesired()
		//
		// Set the far clip distance from the desired far clip distance and the 
		// global quality settings.
		//
		//**************************************
		{
			rFarClipPlaneDist = rDesiredFarClipPlaneDist * 
								qdQualitySettings[iGetQualitySetting()].fFarClipScale;
		}

	//******************************************************************************************
	CCamera::CCamera() 
		: ppmPrediction(0)
	{
		SetFlagShadow(false);
		SetProperties(campropCurrent);
		pcamParent = this;
	}

	//******************************************************************************************
	CCamera::CCamera(const CPresence3<>& pr3, const SProperties& camprop)
		: CEntityAttached(pr3), campropCurrent(camprop), ppmPrediction(0)
	{
		SetFlagShadow(false);
		SetProperties(campropCurrent);
		pcamParent = this;
	}

	//******************************************************************************************
	CCamera::~CCamera()
	{
		delete (CBoundVol*)pbvVolume;
		delete (CBoundVolCamera*)pbvcamClipVolume_;
		delete ppmPrediction;
	}

	//******************************************************************************************
	void CCamera::SetProperties(const CCamera::SProperties& camprop)
	{
		// Copy the new properties into the camera's data member.
		campropCurrent = camprop;

		// Calculate fViewWidthRatio.
		if (camprop.bPerspective)
			campropCurrent.fViewWidthRatio = float(campropCurrent.vpViewport.scWidth) /
			                                 campropCurrent.rViewWidth;
		else
			campropCurrent.fViewWidthRatio = float(campropCurrent.vpViewport.scWidth);

		// Validate the camera's properties.
		Assert(camprop.fAspectRatio > 0);
		Assert(camprop.rViewWidth > 0);

		Assert(camprop.fZoomFactor > 0);

		if (camprop.bPerspective)
			Assert(camprop.rNearClipPlaneDist > 0);
		Assert(camprop.rFarClipPlaneDist  > camprop.rNearClipPlaneDist);

		//
		// Determine the view normalising transform that transforms the view volume into a
		// canonical view volume (cube or pyramid with max extents of 1 in every direction
		// except the near).
		//
		// Scale x by the inverse of the world horizontal extent.  Scale z by the corresponding
		// extent in the vertical direction, scaled by physical aspect ratio.
		//
		double d_scale_x = 1.0 / camprop.rViewWidth;
		double d_scale_z = d_scale_x * camprop.fAspectRatio;


		//
		// Adjust the scale for zoom.
		//
		// The higher the zoom, the less the camera will 'see'. Thus the scale factors need to
		// be increased with increasing zoom.
		//
		d_scale_x *= camprop.fZoomFactor;
		d_scale_z *= camprop.fZoomFactor;

		// Apply the scale that transforms y to a range of Near/Far .. 1.
		double d_scale_y = 1.0 / camprop.rFarClipPlaneDist;

		if (camprop.bPerspective)
		{
			// In normalised volume, the half-width is 1.0 at the far clipping plane.
			d_scale_x *= d_scale_y;
			d_scale_z *= d_scale_y;

			// Calculate the approximate 'distance' (or really 'ratio', since the prespective projection is
			// invariant under uniform scale distance is a relative term) projection plane based on the
			// average of the x and z scale factors relative to the y scale.
			rProjectPlaneDist = (d_scale_z + d_scale_x) * .5 / d_scale_y;
		}

		//
		// Calculate the amount of shear that is required to position the centre of projection
		// on the y axis.  The centre of projection is defined by the centre of the viewport on the display. 
		//
		tf3Normalise = CShear3<>('y', -camprop.vpViewport.vcOriginY, -camprop.vpViewport.vcOriginX) *
					   CScale3<>(d_scale_x, d_scale_y, d_scale_z);

		// Adjust the near clipping plane for the scale in y.
		rNormNearClipDist = camprop.rNearClipPlaneDist * d_scale_y;
		rNormFarClipDist  = 1.0;

		//
		// The inverse z adjust multiplication factor scales 1 / z in screen space to the range
		// [1, near clip dist / far clip dist].
		//
		// Notes:
		//		Previously this value was to be removed as the last vestige of the old software
		//		Z buffer, but now it is used for hardware Z buffers. Do not remove!
		//
		fInvZAdjust = float(0.1f / camprop.rFarClipPlaneDist);

		//
		// Determine the bounding volume of the camera.
		//

		delete (CBoundVol*)pbvVolume;
		delete (CBoundVolCamera*)pbvcamClipVolume_;

		Assert(tf3Normalise.v3Pos.bIsZero());
		CMatrix3<> mx3_denorm = ~tf3Normalise.mx3Mat;

		if (camprop.bPerspective)
		{
			TReal r_inv_far_dist = 1.0 / camprop.rFarClipPlaneDist;

			pbvVolume = pbvcamPerspective
			(
				// (CVector3<>(-1, 1, 0) * mx3_denorm).tX
				(-mx3_denorm.v3X.tX + mx3_denorm.v3Y.tX) * r_inv_far_dist,
				// (CVector3<>(+1, 1, 0) * mx3_denorm).tX
				(+mx3_denorm.v3X.tX + mx3_denorm.v3Y.tX) * r_inv_far_dist,
				// (CVector3<>(0, 1, -1) * mx3_denorm).tZ
				(-mx3_denorm.v3Z.tZ + mx3_denorm.v3Y.tZ) * r_inv_far_dist,
				// (CVector3<>(0, 1, +1) * mx3_denorm).tZ
				(+mx3_denorm.v3Z.tZ + mx3_denorm.v3Y.tZ) * r_inv_far_dist,
				camprop.rNearClipPlaneDist, 
				camprop.rFarClipPlaneDist
			);

			//
			// Also create clipper object.  Why are the bounding volume and clipping objects different?
			// Because the bounding volume is in orthonormal space,
			// while the clipping volume is in normalised space.
			//
			pbvcamClipVolume_ = pbvcamPerspectiveNorm(rNormNearClipDist);
		}
		else if (camprop.bClipNearFar)
		{
			// Create a box.
			// Since it must be centred on the origin, it will extend back farther than we like,
			// but shouldn't matter too much, as it is probably looking straight down anyway.
			pbvVolume = new CBoundVolBox
			(
				CVector3<>(1, 1, 1) * mx3_denorm		// Corner.
			);

			pbvcamClipVolume_ = pbvcamParallelNorm();
		}
		else
		{
			// Create an open parallel box.
			pbvVolume = pbvcamParallel
			(
				-mx3_denorm.v3X.tX, +mx3_denorm.v3X.tX,
				-mx3_denorm.v3Z.tZ, +mx3_denorm.v3Z.tZ
			);

			pbvcamClipVolume_ = pbvcamParallel(-1, 1, -1, 1);
		}

		// Set the projection transformation.  This does NOT use the viewport's virtual origin,
		// since we are transforming from normalised (centred) camera space.
		float f_adjust = 0.02f;
		tlr2Project = CTransLinear2<>
		(
			// Normalised projected camera space, with Y axis flipped.
			CRectangle<>(-1, 1, 2, -2),

			// Screen space.  
			// The points are also translated by an additional .5 in the x and y directions so
			// that the drawing primitives can simply truncate (instead of round) to the centre of
			// an integer pixel.
			CRectangle<>
			(
				0.5 + f_adjust,
				0.5 + f_adjust,
				camprop.vpViewport.scWidth - f_adjust * 2.0f,
				camprop.vpViewport.scHeight - f_adjust * 2.0f
			)
		);
	}

	//******************************************************************************************
	CTransform3<> CCamera::tf3ToNormalisedCamera() const
	{
		// The inverse of the camera's transform transforms world space to camera space. Then,
		// the camera's normalising transform converts camera space to the canonical view volume.
		return ~pr3Presence() * tf3Normalise;
	}

	//******************************************************************************************
	CTransform3<> CCamera::tf3ToHomogeneousScreen() const
	{
		// First obtain the transformation to normalised camera space.
		CTransform3<> tf3 = tf3ToNormalisedCamera();

		// Concatenate with the axis swap and scaling done during projection.
		if (campropCurrent.bPerspective)
			tf3 *= CMatrix3<>
			(
				tlr2Project.tlrX.tScale,  0,                        0,
				tlr2Project.tlrX.tOffset, tlr2Project.tlrY.tOffset, 1,
				0,                        tlr2Project.tlrY.tScale,  0
			) * 
			CScaleI3<>(1.0 / fInvZAdjust);
		else
			tf3 *= CTransform3<>
			(
				CMatrix3<>
				(
					tlr2Project.tlrX.tScale, 0, 0,
					0,                       0, tlr2Project.tlrY.tScale,
					0,                       1, 0
				),
				CVector3<>
				(
					tlr2Project.tlrX.tOffset,
					tlr2Project.tlrY.tOffset,
					0
				)
			);

		return tf3;
	}

	//******************************************************************************************
	CVector3<> CCamera::UnprojectPoint(const CVector3<>& v3_screen) const
	{
		if (campropCurrent.bPerspective)
		{
			float f_z = fInvZAdjust / v3_screen.tZ;

			return CVector3<>
			(
				campropCurrent.vpViewport.vcVirtualX(v3_screen.tX) * f_z,
				f_z,
				campropCurrent.vpViewport.vcVirtualY(v3_screen.tY) * f_z
			);
		}
		else
		{
			return CVector3<>
			(
				campropCurrent.vpViewport.vcVirtualX(v3_screen.tX),
				v3_screen.tZ,
				campropCurrent.vpViewport.vcVirtualY(v3_screen.tY)
			);
		}
	}


	//******************************************************************************************
	void CCamera::ProjectVertices
	(
		CPArray<SRenderVertex> parv_vertices	// Array of vertices to transform in place.
	) const
	{
		// Are there any points to process?
		if (parv_vertices.uLen == 0)
			return;

	#if (TARGET_PROCESSOR == PROCESSOR_K6_3D && VER_ASM)

		// Typedef so that the assembler will accept offsets
		typedef CCamera tdCCamera;
		
		SRenderVertex* prv_curr = parv_vertices; 
		SRenderVertex* prv_end = parv_vertices.end();

		__asm
		{
			mov		ecx,[this]
			lea		ecx,[ecx+240]					;offset to allow small displacements below

			femms									;ensure fast switch if any

			align	16

			movd	mm0,[ecx-240]tdCCamera.tlr2Project.tlrX.tScale
			movd	mm1,[ecx-240]tdCCamera.tlr2Project.tlrY.tScale
			movd	mm2,[ecx-240]tdCCamera.tlr2Project.tlrX.tOffset
			movd	mm3,[ecx-240]tdCCamera.tlr2Project.tlrY.tOffset

			punpckldq mm0,mm1						;m0= tlrY.tScale  | tlrX.tScale
			punpckldq mm2,mm3						;m0= tlrY.tOffset | tlrX.tOffset

			movd	mm3,[ecx-240]tdCCamera.fInvZAdjust
		}

		if (campropCurrent.bPerspective)
		{
			while (prv_curr != prv_end)
			{
				__asm
				{
					align	16

					mov		eax,[prv_curr]			;eax= current vertex ptr
					test	esi,esi					;2-byte NOOP to avoid degraded predecode

					movd	mm6,[eax]SRenderCoord.v3Cam.tY

					movd	mm4,[eax]SRenderCoord.v3Cam.tX

					movd	mm5,[eax]SRenderCoord.v3Cam.tZ
					pfrcp	(m6,m6)					;m6= f_inv_y_curr | f_inv_y_curr

					test	esi,esi					;2-byte NOOP to avoid degraded predecode

					movq	mm7,mm6					;m7= f_inv_y_curr | f_inv_y_curr
					pfmul	(m6,m0)					;m6= tlrY|tlrX.tScale * f_inv_y_curr

					punpckldq mm4,mm5				;m4= v3Cam.tZ | v3Cam.tX
					pfmul	(m7,m3)					;m7= f_inv_y_curr * fInvZAdjust

					movd	[eax]SRenderCoord.v3Screen.tZ,mm7
					pfmul	(m4,m6)					;m4= tZ|tX * tlrY|tlrX.tScale * f_inv_y_curr

					pfadd	(m4,m2)					;m4= ditto + tlrY|tlrX.tOffset
					movq	[eax]SRenderCoord.v3Screen.tX,mm4
				}
				
				prv_curr++;
			}
		}
		else
		{
			while (prv_curr != prv_end)
			{
				__asm
				{
					align	16

					mov		eax,[prv_curr]			;eax= current vertex ptr
					test	esi,esi					;2-byte NOOP to avoid degraded predecode

					movd	mm4,[eax]SRenderCoord.v3Cam.tX

					movd	mm5,[eax]SRenderCoord.v3Cam.tZ

					movd	mm6,[eax]SRenderCoord.v3Cam.tY

					punpckldq mm4,mm5				;m4= v3Cam.tZ | v3Cam.tX
					movd	[eax]SRenderCoord.v3Screen.tZ,mm6

					pfmul	(m4,m0)					;m4= tZ|tX * tlrY|tlrX.tScale
					nop								;1-byte NOOP to avoid degraded predecode

					pfadd	(m4,m2)					;m4= ditto + tlrY|tlrX.tOffset
					movq	[eax]SRenderCoord.v3Screen.tX,mm4
				}
				
				prv_curr++;
			}
		}

		__asm	femms							//empty MMX state and ensure fast switch if any

	#else // (TARGET_PROCESSOR == PROCESSOR_K6_3D) && VER_ASM

		if (campropCurrent.bPerspective)
		{
			// Switch floating point unit to low precision mode.
			CFPUState fpus;

			fpus.SetTruncate();

			for (SRenderVertex* prv_curr = parv_vertices; prv_curr != parv_vertices.end(); prv_curr++)
			{
				float f_inv_y_curr = 1.0 / prv_curr->v3Cam.tY;

				// Project to screen space.
				prv_curr->v3Screen.tX = (prv_curr->v3Cam.tX * f_inv_y_curr) * tlr2Project.tlrX;
				prv_curr->v3Screen.tY = (prv_curr->v3Cam.tZ * f_inv_y_curr) * tlr2Project.tlrY;
				prv_curr->v3Screen.tZ = f_inv_y_curr * fInvZAdjust;
			}
		}
		else
		{
			for (SRenderVertex* prv_curr = parv_vertices; prv_curr < parv_vertices.end(); prv_curr++)
			{
				prv_curr->v3Screen = ProjectPointParallel(prv_curr->v3Cam);
			}
		}

	#endif // (TARGET_PROCESSOR == PROCESSOR_K6_3D) && VER_ASM
	}

	//******************************************************************************************
	void CCamera::ProjectPointsPerspective(CPArray< CVector3<> > pav3_from,
		                                   CPArray< CVector3<> > pav3_to,
								           const CTransform3<>&  t3f) const
	{
		// Do nothing if there are no points to project.
		if (pav3_from.uLen < 1)
			return;

		Assert(pav3_from.uLen == pav3_to.uLen);
		Assert(campropCurrent.bPerspective);

		// Get a pointer to the address of the element in the destination array.
		CVector3<>* pv3_dest = pav3_to;

		// Iterate through the points and project them.
		for (uint u = 0; u < pav3_from.uLen; ++u, ++pv3_dest)
		{
			// Transform the point.
			CVector3<> v3 = pav3_from[u];
			v3 = v3 * t3f;

			// Perform the inverse y calculation.
			float f_inv_y = 1.0f / v3.tY;

			// Project to screen space.
			pv3_dest->tX = (v3.tX * f_inv_y) * tlr2Project.tlrX;
			pv3_dest->tY = (v3.tZ * f_inv_y) * tlr2Project.tlrY;
			pv3_dest->tZ = f_inv_y * fInvZAdjust;
		}
	}

	//******************************************************************************************
	void CCamera::ProjectPointsPerspectiveFast(CPArray< CVector3<> > pav3_from,
		                                       CPArray< CVector3<> > pav3_to,
								               const CTransform3<>&  t3f) const
	{
		// Do nothing if there are no points to project.
		if (pav3_from.uLen < 1)
			return;

		Assert(pav3_from.uLen == pav3_to.uLen);
		Assert(campropCurrent.bPerspective);

		// Get a pointer to the address of the element in the destination array.
		CVector3<>* pv3_dest = pav3_to;

		// Iterate through the points and project them.
		for (uint u = 0; u < pav3_from.uLen; ++u, ++pv3_dest)
		{
			// Transform the point.
			CVector3<> v3 = pav3_from[u] * t3f;

			// Perform the inverse y calculation.
			float f_inv_y = fInverse(v3.tY);

			// Project to screen space.
			pv3_dest->tX = (v3.tX * f_inv_y) * tlr2Project.tlrX;
			pv3_dest->tY = (v3.tZ * f_inv_y) * tlr2Project.tlrY;
			pv3_dest->tZ = f_inv_y * fInvZAdjust;
		}
	}
	
	//******************************************************************************************
	CVolSphere CCamera::vsBoundingSphere() const
	{
		CSArray<CVector3<>, 8> sa_extents;
		WorldExtents(sa_extents);

		// Find an approximate bounding sphere to the camera volume.

		// Put the centre of the sphere at the geometric average of the world extents.
		CVector3<> v3_avg(0, 0, 0);

		for (int i_avg = 0; i_avg < 8; i_avg++)
			v3_avg += sa_extents[i_avg];

		v3_avg /= 8;

		// Find the maximum squared radius of the sphere.
		TReal r_radius_sqr = 0;

		for (int i_radius = 0; i_radius < 8; i_radius++)
			r_radius_sqr = Max(r_radius_sqr, (sa_extents[i_radius] - v3_avg).tLenSqr());

		return CVolSphere(sqrt(r_radius_sqr), v3_avg);
	}

	//******************************************************************************************
	void CCamera::WorldExtents(CPArray< CVector3<> > pa_extents) const
	{
		// Currently implemented only for perspective cameras!
		Assert(campropCurrent.bPerspective);

		// First, define the extents of the camera's volume in normalised camera space.
		pa_extents[0] = CVector3<>(-1, rNormFarClipDist, -1);
		pa_extents[1] = CVector3<>( 1, rNormFarClipDist, -1);
		pa_extents[2] = CVector3<>( 1, rNormFarClipDist,  1);
		pa_extents[3] = CVector3<>(-1, rNormFarClipDist,  1);

		TReal r_near_far_ratio = rNormNearClipDist / rNormFarClipDist;

		pa_extents[4] = CVector3<>(-r_near_far_ratio, rNormNearClipDist, -r_near_far_ratio);
		pa_extents[5] = CVector3<>( r_near_far_ratio, rNormNearClipDist, -r_near_far_ratio);
		pa_extents[6] = CVector3<>( r_near_far_ratio, rNormNearClipDist,  r_near_far_ratio);
		pa_extents[7] = CVector3<>(-r_near_far_ratio, rNormNearClipDist,  r_near_far_ratio);

		// Convert the normalised extents to world space.
		CTransform3<> tf3_norm_to_world = ~tf3Normalise * pr3Presence();

		for (int i = 0; i < 8; i++)
			pa_extents[i] *= tf3_norm_to_world;
	}


	//*****************************************************************************************
	bool CCamera::bCanHaveChildren()
	{
		return false;
	}

	//*****************************************************************************************
	void CCamera::CreatePrediction()
	{
		// Do nothing if there is already a prediction object.
		if (ppmPrediction)
			return;

		// Create a prediction object.
		ppmPrediction = new CPredictMovement(pr3Presence());
	}
	
	//*****************************************************************************************
	void CCamera::UpdatePrediction()
	{
		// Create a prediction object if there is not already one.
		CreatePrediction();

		// Call the update member function on the prediction object.
		ppmPrediction->UpdatePrediction(pr3Presence());
	}

	//*****************************************************************************************
	CPredictMovement* CCamera::ppmGetPrediction()
	{
		// Create a prediction object if there is not already one.
		CreatePrediction();

		// Return the pointer to the prediction object.
		return ppmPrediction;
	}

	//*****************************************************************************************
	void CCamera::ClearPrediction()
	{
		// Do nothing if there is no prediction object.
		if (!ppmPrediction)
			return;

		// Call the clear member function on the prediction object.
		ppmPrediction->ClearTracking();
	}

	//*****************************************************************************************
	CPresence3<> CCamera::pr3GetStandardPrediction()
	{
		// Create a prediction object if there is not already one.
		CreatePrediction();

		// Return a presence predicting the future presence of the object.
		return ppmPrediction->pr3GetStandardPrediction();
	}

	//*****************************************************************************************
	CPresence3<> CCamera::pr3VPresence(bool b_use_prediction)
	{
		// Use prediction if requested and available.
		if (b_use_prediction && ppmPrediction)
			return ppmPrediction->pr3GetStandardPrediction();

		// Otherwise just use the camera's current presence.
		return pr3Presence();
	}

	//*****************************************************************************************
	CPresence3<> CCamera::pr3VPresence(bool b_use_prediction) const
	{
		// Use prediction if requested and available.
		if (b_use_prediction && ppmPrediction)
			return ppmPrediction->pr3GetStandardPrediction();

		// Otherwise just use the camera's current presence.
		return pr3Presence();
	}

	//*****************************************************************************************
	char *CCamera::pcSave(char * pc) const
	{
		// Instead of calling parent, simply save presence, because CInstance only
		// saves those instances with physics properties.
		pc = pr3Pres.pcSave(pc);

		// Save version of properties.
		*((int *)pc) = 'VER1';
		pc += sizeof(int);

		// Now save the properties!
		SProperties sprops = campropGetProperties();
		*((SProperties*)pc) = sprops;
		pc += sizeof(SProperties);

		return pc;
	}

	// Hack - these are here temporarily in order to not break scene loading.
	struct SPropertiesOld
	{
		CViewport vpViewport;
		bool bPerspective;
		TReal rNearClipPlaneDist;
		TReal rFarClipPlaneDist;
		bool bClipNearFar;
		TReal rViewWidth;
		float fAspectRatio;
		float fZoomFactor;
		float fViewWidthRatio;
	};

	//*****************************************************************************************
	const char* CCamera::pcLoad(const char* pc)
	{
		pc = pr3Pres.pcLoad(pc);

		// Get Current settings.
		SProperties sprops_current =	campropGetProperties();

		if (*((int *)pc) == 'VER1')
		{
			// New properties.
			pc += sizeof(int);

			// Load saved settings!
			SProperties sprops_loaded = *((SProperties*)pc);
			pc += sizeof(SProperties);

			// Set the particular fields that we actually want to load.
			sprops_current.rNearClipPlaneDist = sprops_loaded.rNearClipPlaneDist;
			sprops_current.rDesiredFarClipPlaneDist = sprops_loaded.rDesiredFarClipPlaneDist;
			sprops_current.SetFarClipFromDesired();
		}
		else
		{
			// Old properties.

			// Load saved settings!
			SPropertiesOld sprops_loaded = *((SPropertiesOld*)pc);
			pc += sizeof(SPropertiesOld);

			// Set the particular fields that we actually want to load.
			sprops_current.rNearClipPlaneDist = sprops_loaded.rNearClipPlaneDist;
			sprops_current.rDesiredFarClipPlaneDist = sprops_loaded.rFarClipPlaneDist;
			sprops_current.SetFarClipFromDesired();
		}

		// Set the camera with the new properties.
		SetProperties(sprops_current);

		return pc;
	}
