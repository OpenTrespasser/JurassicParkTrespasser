/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Implementation of Light.hpp.
 *
 * Bugs:
 *
 * To do:
 *		Figure how how rvEmissive works...currently modulated by rvDiffuse.
 *		Remove the current 'hierarchy kludge'.
 *		Make CLight always attached to CInstance when the render type is added to the instance;
 *		rather than letting CLightList do it later.
 *		Implement the bounding volume function. It currently simply returns an infinite volume
 *		in all cases.
 *		Change PointDir to have the functionality of a spot light; either via a virtual radius,
 *		which affects angle and distance calculations; or a spot light diameter and divergence
 *		angle.
 *		Change lighting model to incorporate optional coloured lights.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/Light.cpp                                                $
 * 
 * 53    98/08/28 12:08 Speter
 * Redid normal tolerance to prevent assert.
 * 
 * 52    98/08/22 21:47 Speter
 * Further safety for light dot-product ranges.
 * 
 * 51    8/11/98 2:16p Agrant
 * changed include to allow compilation
 * 
 * 50    98/07/22 14:46 Speter
 * Update for new CClut functions: TReflectVal now calculated by CMaterial. Slightly improved
 * diffuse/specular combining code. Added fudge code for guarding against dot-product range
 * errors. Added Assert for range error.
 * 
 * 49    98/06/01 18:47 Speter
 * Removed position and direction arguments from bltGetBumpLighting, as they are ignored.
 * 
 * 48    3/18/98 4:07p Pkeet
 * Added the 'PartitionPriv.hpp' include.
 * 
 * 47    98/02/26 14:00 Speter
 * Added "Clut.hpp" due to reduced include dependency.
 * 
 * 46    2/05/98 12:12p Agrant
 * Lighting calculations make up some defaults if there are no lights.
 * 
 * 45    97/10/12 22:06 Speter
 * Moved lvSpecularLighting() functionality to CMaterial.  Changed rvGetLighting() to
 * cvGetLighting(), returning scaled clut indices; makes use of clut functions.
 * 
 * 44    97/09/29 16:25 Speter
 * All bounding volumes are now positionless.  Removed v3Pos from Box and Sphere.   Must remove
 * this shadow code.
 * 
 * 43    97/09/16 15:48 Speter
 * Incorporated simpler SBumpLighting structure into lighting.  Now fold diffuse/specular
 * together in lighting.  Removed SDirReflectData and SPrimaryReflectData structures.  Changed
 * prdGetPrimaryData() so it returns a SBumpLighting structure.  
 * 
 * 42    97/09/11 17:36 Speter
 * Bump lighting now works better with multiple lights, creating a composite light from them
 * rather than picking one.  Eye direction is now passed to SetViewingContext, not lighting
 * calls.  Ambient now contributes to specular shading as well.
 * Moved To do's to .cpp file.
 * 
 * 41    97/09/08 17:26 Speter
 * CLightDir is again inherited from CLightAmbient (convenient to share lvIntensity member).
 * 
 * 40    97/07/07 14:14 Speter
 * Changed UpdateShadows to accept a single top-level partition rather than a  bool and bounding
 * volume.  Now passes top-level partition to CShadowBuffer::Render(), rather than a partition
 * list.
 * 
 * 39    97/06/02 13:59 Speter
 * Added handy flags for determining whether lights have directional and positional dependence.
 * 
 * 38    97-04-03 19:13 Speter
 * Changed pinsParent to ppr3Presence.  Moved UpdateShadows() down to CLight.  Removed setting
 * of parent in CLightList; now done by CEntityLight.
 * 
 * 37    97-03-28 16:17 Speter
 * First somewhat working shadow implementation.
 * Added code to interface with shadow buffers, and update when objects are added.
 * 
 **********************************************************************************************/

#include "Common.hpp"
#include "Light.hpp"
#include "Lib/GeomDBase/PartitionPriv.hpp"

#include "Shadow.hpp"
#include "Lib/View/Clut.hpp"
//#include "Lib/EntityDBase/RenderDB.hpp"
#include "Lib/EntityDBase/Query/QRenderer.hpp"
#include "Lib/GeomDBase/PartitionSpace.hpp"

#define bBUMP_LIGHT_PER_OBJ	1
#define bDIR_PER_OBJ		0
#define iSHADOW_BUFFER_DIM	256

//**********************************************************************************************
//
// CLightDir implementation.
//

	//******************************************************************************************
	void CLightDir::SetViewingContext(const CPresence3<>& pr3)
	{
		// Tell the shadow buffer about our current transform, for quick shadow tests.
		if (pShadowBuffer)
			pShadowBuffer->SetViewingContext(pr3);
	}

//**********************************************************************************************
//
// CLightDirectional implementation.
//

	//******************************************************************************************
	void CLightDirectional::SetViewingContext(const CPresence3<>& pr3)
	{
		CLightDir::SetViewingContext(pr3);

		//
		// Find the direction of light emanation in the transformed space,
		// and negate it to point toward the light.
		// Reduce the direction's magnitude slightly to ensure no lighting
		// dot-products ever go above 1.
		//
		d3Dir = CDir3<>(d3ZAxis * (ppr3Presence->r3Rot * pr3.r3Rot) * -0.999, true);

		// Cache the strength*direction vector, for efficient calculations.
		// Reduce the magnitude even more, to prevent overflows.
		v3Strength = d3Dir * (lvIntensity * .995);
	}

	//******************************************************************************************
	void CLightDirectional::UpdateShadows(CPartition* ppart)
	{
		if (!bShadowing)
			return;

		if (!ppart)
			// Delete any shadow buffer.
			pShadowBuffer = rptr0;
		else if (!pShadowBuffer)
		{
			// Create and calculate the shadow buffer.
			// Currently, we must extract the true world extents of the partition, rather than
			// using its bounding volume, which may be infinite.
			//
			CVector3<> v3_min, v3_max;
			Verify(ppart->bGetWorldExtents(v3_min, v3_max));

			// To do: fix/remove this when shadowing re-implemented.
			CBoundVolBox bvb((v3_max - v3_min) * 0.5);

			pShadowBuffer = rptr_new CShadowBuffer
			(
				iSHADOW_BUFFER_DIM,			// How many pixels to make the shadow buffer.
				bvb,						// Bounding volume to enclose.
				ppr3Presence->r3Rot			// Placement of this light.
			);

			// Render these shapes.
			pShadowBuffer->Render(ppart);
		}
	}

//**********************************************************************************************
//
// CLightPoint implementation.
//

	//******************************************************************************************
	void CLightPoint::SetViewingContext(const CPresence3<>& pr3)
	{
		CLightDir::SetViewingContext(pr3);

		// Set the position in transformed space.
		v3Position = ppr3Presence->v3Pos * pr3;

		// Store radius in object space.
		rObjRadius = rRadius * pr3.rScale;
	}

	//******************************************************************************************
	SLightInfo CLightPoint::ltiGetInfo(const CVector3<>& v3) const
	{
		CVector3<>	v3_to_light = v3Position - v3;
		TReal		r_dist		= v3_to_light.tLen();
		CDir3<>		d3_to_light;
#if bDIR_PER_OBJ
		// The direction is always taken from the origin in the transformed space.
		if (!v3Position.bIsZero())
			d3_to_light = v3Position;
#else
		if (r_dist)
			d3_to_light = CDir3<>(v3_to_light / r_dist, true);
#endif
		//
		// Scale by the inverse square of the distance from the light.
		// Also scale by an amount sufficient to make the strength 1 at rObjRadius,
		// and clamp to 1 inside rObjRadius.
		//

		TReal		r_sin = Min(rObjRadius / r_dist, 1);
		TLightVal	lv_strength = r_sin * r_sin * lvIntensity;

		// The width is determined from r_sin.
		TAngleWidth		angw_size = angwFromSin(r_sin);

		return SLightInfo(lv_strength, d3_to_light, angw_size);
	}


//**********************************************************************************************
//
// CLightPointDirectional implementation.
//

	//******************************************************************************************
	void CLightPointDirectional::SetViewingContext(const CPresence3<>& pr3)
	{
		// Set the position in transformed space.
		CLightPoint::SetViewingContext(pr3);

		// Also calculate the direction;
		// Set to the direction of Z in the transformed space, negated to point toward the light.
		d3Direction = -(d3ZAxis * (ppr3Presence->r3Rot * pr3.r3Rot));
	}

	//******************************************************************************************
	SLightInfo CLightPointDirectional::ltiGetInfo(const CVector3<>& v3) const
	{
		SLightInfo lti = CLightPoint::ltiGetInfo(v3);

		// Find the farness of the angle of incidence to the light direction.
		TReal r_cos = lti.d3Light * d3Direction;

		// Scale the strength according to the directionality of the light.
		lti.lvStrength *= fAngularStrength(r_cos, angwInner, angwOuter);

		return lti;
	}

//**********************************************************************************************
//
// CLightList implementation.
//

	//******************************************************************************************
	CLightList::CLightList(const std::list<CInstance*>& listins_lights)
	{
		lvAmbient = 0;

		std::list<CInstance*>::const_iterator itins = listins_lights.begin();
		for ( ; itins != listins_lights.end(); itins++)
		{
			// Retrieve the light from the instance.
			rptr<CLight> plt = ptCastRenderType<CLight>((*itins)->prdtGetRenderInfo());
			Assert(plt);

			rptr<CLightDir> pltd = rptr_dynamic_cast(CLightDir, plt);
			if (pltd)
			{
				rptr<CLightDirectional> pltdir = rptr_dynamic_cast(CLightDirectional, plt);
				if (pltdir)
					// Add to directional list.
					listpltDir.push_back(pltdir);
				else
				{
					// Add to non-dir list.
					listpltPos.push_back(pltd);
				}

				listpltAll.push_back(pltd);
			}
			else
			{
				// Add ambient light strength to total.
				rptr<CLightAmbient> plta = rptr_dynamic_cast(CLightAmbient, plt);
				if (plta)
					lvAmbient = fCombine(lvAmbient, plta->lvIntensity);
			}
		}
	}

	//******************************************************************************************
	void CLightList::SetViewingContext(const CPresence3<>& pr3, const CPresence3<>& pr3_eye)
	{
		// Set the viewing context in each of the children.
		forall (listpltAll, std::list< rptr<CLightDir> >, it)
		{
			(*it)->SetViewingContext(pr3);
		}

		// Set the eye direction in object space, for specular calculations in bump mapping only.
		// It's relative to the origin of the object, and is assumed constant across the object.
		if (pr3_eye.v3Pos.bIsZero())
			// Set the direction to negative Y in camera space.
			d3Eye = -d3YAxis * pr3_eye.r3Rot;
		else
			d3Eye = pr3_eye.v3Pos;
	}

	//******************************************************************************************
	void CLightList::UpdateShadows(CPartition* ppart)
	{
		// Update the shadows of all children.
		forall (listpltAll, std::list< rptr<CLightDir> >, it)
		{
			(*it)->UpdateShadows(ppart);
		}
	}

	//******************************************************************************************
	TClutVal CLightList::cvGetLighting(const CVector3<>& v3_pos, const CDir3<>& d3_normal, 
		const CClut& clut) const
	{
		const CMaterial& mat = *clut.pmatMaterial;

		if (mat.bRefractive)
		{
			// Ignore lights, and just return the reflectance at the given viewer angle.
			// To do: perform separate specular calculation, and add in.
			TReflectVal rv = mat.fReflected(d3Eye * d3_normal);
			return clut.cvFromReflect(rv);
		}

		// Sum diffuse lighting for all components.  Initialise with ambient value.
		TReflectVal rv = 0.0;
		TLightVal lv_diffuse = lvAmbient;

		if (mat.rvSpecular)
		{
			TLightVal lv_specular = 0;

			forall_const (listpltAll, std::list< rptr<CLightDir> >, it)
			{
				if ((*it)->bShadowed(v3_pos))
					continue;

				// Retrieve the light info at this point in space.
				SLightInfo lti = (*it)->ltiGetInfo(v3_pos);

				// Calculate the incidence on the surface.
				TLightVal lv = lti.d3Light * d3_normal;
				if (lv > 0)
				{
					// A positive light incidence.  Add to lv_diffuse, and calculate specular.
					lv_diffuse = fCombine(lv_diffuse, lv * lti.lvStrength);

					// Calculate specular lighting.  The direction of maximum specular highlight 
					// is midway between the eye and the light directions.
					CDir3<> d3_specular = lti.d3Light + d3Eye;

					// The specular intensity depends on the nearness of the surface normal to the specular
					// highlight.
					lv = mat.fSpecular(d3_normal * d3_specular, lti.angwSize);
					if (lv > 0)
						// Scale by light strength, and combine.
						lv_specular = fCombine(lv_specular, lv * lti.lvStrength);
				}
			}

			// Scale diffuse and specular lighting values by material reflectance,
			// and return their combined reflectance.
			rv = mat.rvCombined(lv_diffuse, lv_specular);
		}
		else
		{
			// Handle Directional lights explicitly, avoiding a call to ltiGetInfo().
			forall_const (listpltDir, std::list< rptr<CLightDirectional> >, it)
			{
				if ((*it)->bShadowed(v3_pos))
					continue;

				TLightVal lv_dir = d3_normal * (*it)->v3Strength;

				// Add to lv_diffuse only if positive.
				if (lv_dir >= 0)
					lv_diffuse = fCombine(lv_diffuse, lv_dir);
			}

			// Do the remaining lights.
			forall_const (listpltPos, std::list< rptr<CLightDir> >, itd)
			{
				if ((*itd)->bShadowed(v3_pos))
					continue;

				// Retrieve the light info at this point in space.
				SLightInfo lti = (*itd)->ltiGetInfo(v3_pos);

				// Calculate the incidence on the surface.
				TLightVal lv = lti.d3Light * d3_normal;
				if (lv > 0)
					// A positive light incidence.  Add to lv_diffuse.
					lv_diffuse = fCombine(lv_diffuse, lv * lti.lvStrength);
			}

			rv = mat.rvCombined(lv_diffuse, 0);
		}

		if (mat.rvEmissive)
			rv = fCombine(rv, mat.rvEmissive, rvMAX_WHITE);

		// Convert reflectance to clut value.
		return clut.cvFromReflect(rv);
	}

	//******************************************************************************************
	SBumpLighting CLightList::bltGetBumpLighting(const CMaterial& mat) const
	{
		// Fake position at object origin.
		CVector3<> v3_pos(0, 0, 0);

		SBumpLighting	blt;

		// Bump-mapping works with one directional light only.
		// Calculate composite light from combination of all directionals.
		if (listpltAll.size() == 1)
		{
			if ((*listpltAll.begin())->bShadowed(v3_pos))
				blt.lvStrength = 0;
			else
				static_cast<SLightInfo&>(blt) = (*listpltAll.begin())->ltiGetInfo(v3_pos);
			blt.lvAmbient = lvAmbient;
		}
		else
		{
			CVector3<> v3_combined(0, 0, 0);		// Vector sum of lights.
			TLightVal lv_sum = 0;					// Simple summed strength of lights.
			TLightVal lv_combined = 0;				// Combined strength (via fCombine).
			TAngleWidth angw_combined = 0;			// Combined angle width.

			// To do: perform this calculation in SetViewingContext, use when no positional info is needed.
			// Do we have some lights?
			if (listpltAll.size() != 0)
			{
				// Yes!  Add them up.
				forall_const (listpltAll, std::list< rptr<CLightDir> >, it)
				{
					if ((*it)->bShadowed(v3_pos))
						continue;
					SLightInfo lti = (*it)->ltiGetInfo(v3_pos);
					v3_combined += lti.d3Light * lti.lvStrength;
					lv_sum += lti.lvStrength;
					lv_combined = fCombine(lv_combined, lti.lvStrength);
					angw_combined += lti.angwSize * lti.lvStrength;
				}
			}
			else
			{	
				// No!  Make up some bogus data.
				v3_combined = CVector3<>(0,0,-1);
				lv_sum = 1.0f;
			}

			//
			// lv_combined is the total light strength.  It is split into two components,
			// a base and directional.  The directional is determined by the vector sum
			// of the lighting values, scaled by the ratio of lv_combined to lv_sum.
			// The base is the difference between the directional magnitude and lv_combined.
			//

			// The composite light strength must be modified by the combination values.
			TLightVal lv_inv_sum = 1.0 / lv_sum;
			TLightVal lv_mag = v3_combined.tLen();

			blt = SBumpLighting
			(
				0,										// 0 as base for now.
				lv_mag * lv_combined * lv_inv_sum,		// Strength of directional component.
				CDir3<>(v3_combined / lv_mag, true),	// Direction (use fast constructor).
				angw_combined * lv_inv_sum				// Average angle size.
			);

			// Set ambient value, incorporating ambient light, and base non-directional light
			// for both diffuse and specular.
			blt.lvAmbient = fCombine(lvAmbient, lv_combined - blt.lvStrength);
		}

		// Adjust directional strength a la fCombine.
		blt.lvStrength *= 1.0 - lvAmbient;

		if (mat.bRefractive)
		{
			//
			// For reflection mapping with bumps, the calculations are based upon
			// viewer angle, not light angle.
			//
			blt.d3Light = d3Eye;
		}

		else if (mat.rvSpecular)
		{
			// Calculate specular light direction, then average into composite direction.
			CDir3<> d3_specular = d3Eye + blt.d3Light;
			blt.d3Light = blt.d3Light * mat.rvDiffuse + d3_specular * mat.rvSpecular;
			Assert(Abs(blt.d3Light.tZ) <= 1.0f);
		}

		return blt;
	}

	//******************************************************************************************
	SBumpLighting CLightList::bltGetPrimaryBumpLighting() const
	{
		SBumpLighting blt;

		// Do not combine lights; just find primary one.
		blt.lvAmbient = lvAmbient;

		// Iterate through directional lights, finding strongest.
		forall_const (listpltDir, std::list< rptr<CLightDirectional> >, it)
			SetMax(blt.lvStrength, (*it)->lvIntensity);

		// We do not need light direction or width; just ignore these.
		blt.lvStrength *= (1.0 - blt.lvAmbient);
		return blt;
	}

