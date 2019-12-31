/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		SLightInfo
 *		CLight
 *			CLightAmbient
 *				CLightDir
 *					CLightDirectional
 *					CLightPoint
 *						CLightPointDirectional
 *		CLightList
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/Light.hpp                                                $
 * 
 * 50    98/06/01 18:47 Speter
 * Removed position and direction arguments from bltGetBumpLighting, as they are ignored.
 * 
 * 49    98/02/26 13:59 Speter
 * Added CClut, due to reduced include dependency.
 * 
 * 48    97/10/12 21:56 Speter
 * lvCombine moved here.  Changed rvGetLighting() to cvGetLighting(), returning scaled clut
 * indices.
 * 
 * 47    97/09/16 15:47 Speter
 * Incorporated simpler SBumpLighting structure into lighting.  Removed SDirReflectData and
 * SPrimaryReflectData structures.  Changed prdGetPrimaryData() so it returns a SBumpLighting
 * structure.  Moved TAngleWidth constants to Material.hpp.  
 * 
 * 46    97/09/11 17:35 Speter
 * Bump lighting now works better with multiple lights, creating a composite light from them
 * rather than picking one.  Eye direction is now passed to SetViewingContext, not lighting
 * calls.
 * Moved To do's to .cpp file.
 * 
 * 45    97/09/08 17:26 Speter
 * CLightDir is again inherited from CLightAmbient (convenient to share lvIntensity member).
 * 
 * 44    97/07/07 14:13 Speter
 * Changed UpdateShadows to accept a single top-level partition rather than a  bool and bounding
 * volume.
 * 
 * 43    97/06/02 13:58 Speter
 * Added handy flags for determining whether lights have directional and positional dependence.
 * 
 * 42    5/30/97 11:05a Agrant
 * Goofy LINT fixes.
 * 
 * 41    97-04-03 19:12 Speter
 * Changed pinsParent to ppr3Presence.  Moved UpdateShadows() down to CLight.
 * 
 * 40    97-03-28 16:17 Speter
 * First somewhat working shadow implementation.
 * Added code to interface with shadow buffers, and update when objects are added.
 * 
 * 39    97/03/24 15:05 Speter
 * Added CLightDir intermediate class for all non-ambient lights, simplified SLightInfo
 * structure.
 * Made CLightList no longer a CLight.
 * Added some shadow support infrastructure to lights.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_RENDERER_LIGHT_HPP
#define HEADER_LIB_RENDERER_LIGHT_HPP

#include "RenderType.hpp"
#include "RenderDefs.hpp"
#include "GeomTypes.hpp"
#include "Shadow.hpp"

#include <list>

//
// Opaque declarations.
//

class CInstance;
class CClut;

//
// Constants.
//

//**********************************************************************************************
//
inline TLightVal lvCombine
(
	TLightVal lv_diffuse,
	TLightVal lv_specular,
	const CMaterial& mat = matDEFAULT
)
//
// Returns:
//		A composite light value usable as a relative clut index.
//
//**************************************
{
	return Max(lv_diffuse * mat.angwSpecular, lv_specular);
}

//**********************************************************************************************
//
class CLight : public CRenderType
//
// Prefix: lt
//
//**************************************
{
public:
	ptr_const< CPresence3<> >	ppr3Presence;	// The presence to which the light is attached
												// (normally the presence of an instance).
protected:
	CBoundVolInfinite bviVolume;

public:

	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	virtual bool bIsDirectional() const
	//
	// Returns:
	//		Whether this light is direction-dependent (i.e. non-ambient).
	//
	//**********************************
	{
		return false;
	}

	//******************************************************************************************
	//
	virtual bool bIsPositional() const
	//
	// Returns:
	//		Whether this light is position-dependent (i.e. it has a location).
	//
	//**********************************
	{
		return false;
	}

	//******************************************************************************************
	//
	virtual void SetViewingContext
	(
		const CPresence3<>& pr3		// Coordinate space in which to perform calculations.
									// (typically a world-to-object transformation).
	)
	//
	// Set any internal parameters associated with the viewing context.
	//
	//**********************************
	{
	}

	//******************************************************************************************
	//
	virtual SLightInfo ltiGetInfo
	(
		const CVector3<>& v3				// The point at which to calculate the info.
	) const
	//
	// Returns:
	//		The light strength, direction, and optionally, width, at the given point.
	//
	// Cross-references:
	//		This function is used by other member routines which calculate the actual light
	//		incident on a surface.
	//
	//**********************************
	{
		// I want to make this a pure virtual, but the linker says it isn't defined!
		// Why oh why does this happen?
		return SLightInfo();
	}

	//******************************************************************************************
	//
	virtual void UpdateShadows
	(
		CPartition* ppart				// Top level of the partition tree to render shadows for,
										// or 0 to remove shadow buffer.
	)
	//
	// Update the light's shadow buffer with the shadows of the shapes in the given partition,
	// or remove it if ppart is 0.
	//
	//**********************************
	{
	}

	//******************************************************************************************
	//
	// Overrides.
	//

	//******************************************************************************************
	virtual const CBoundVol& bvGet() const
	{
		// Not implemented yet.
		return bviVolume;
	}

	//******************************************************************************************
	virtual void Cast(rptr_const<CLight>* pplt) const
	{
		*pplt = rptr_const_this(this);
	}

	//******************************************************************************************
	virtual rptr<CRenderType> prdtCopy()
	{
		// Not implemented yet.
		Assert(false);
		return rptr0;
	}

	// Let the CLightList class call the protected members of its children.
	friend class CLightList;
};


//**********************************************************************************************
//
class CLightAmbient : public CLight
//
// Prefix: lta
//
// A light which has a constant value everywhere, and no directionality.
//
//**************************************
{
public:
	TLightVal lvIntensity;		// The intensity of the light.

public:

	//******************************************************************************************
	//
	// Constructor.
	//

	CLightAmbient(TLightVal lv_intensity)
		: lvIntensity(lv_intensity)
	{
		Assert(bWithin(lvIntensity, 0, 1));
	}

	//******************************************************************************************
	//
	// Overrides.
	//

	virtual SLightInfo ltiGetInfo(const CVector3<>&) const
	{
		// Returns constant, non-directional light.
		return SLightInfo(lvIntensity);
	}
};


//**********************************************************************************************
//
class CLightDir : public CLightAmbient
//
// Prefix: lta
//
// A light which has a directional component.
//
//**************************************
{
public:
	bool			bShadowing;			// This is a shadowing light, and has an attached shadow buffer.
	rptr<CShadowBuffer>	pShadowBuffer;	// The shadow buffer, if shadowing.

public:

	//******************************************************************************************
	//
	// Constructor.
	//

	CLightDir(TLightVal lv_intensity, bool b_shadowing = false)
		: CLightAmbient(lv_intensity), bShadowing(b_shadowing)
	{
		Assert(bWithin(lvIntensity, 0, 1));
	}

	//******************************************************************************************
	//
	// Overrides.
	//

	virtual bool bIsDirectional() const
	{
		return true;
	}

	virtual void SetViewingContext(const CPresence3<>& pr3);

	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	bool bShadowed
	(
		const CVector3<>& v3_world		// Location in the world.
	) const
	//
	// Returns:
	//		Whether the given point is in shadow for this light.
	//
	//**********************************
	{
		return pShadowBuffer && pShadowBuffer->bShadowed(v3_world);
	}
};

//**********************************************************************************************
//
class CLightDirectional: public CLightDir
//
// Prefix: ltd
//
// A light which emanates from a certain direction, but is considered to be an infinite distance,
// so that lighting does not depend on location.
//
// The direction of light emanation is considered to be the light's z-axis as transformed by its
// placement.  The position component of the placement is ignored.
//
//**************************
{
protected:
	CDir3<>		d3Dir;		// The light's direction, negated to point *toward* the light.
	CVector3<>	v3Strength;	// Direction scaled by intensity; saves a multiply in calculations.
	TAngleWidth	angwSize;	// The cosine of the angular extent of this light in the sky.

public:

	//******************************************************************************************
	//
	// Constructor.
	//

	CLightDirectional(TLightVal lv_intensity, bool b_shadowing = false, TAngleWidth angw_size = angwDEFAULT_SIZE)
		: CLightDir(lv_intensity, b_shadowing), angwSize(angw_size)
	{
	}

	//******************************************************************************************
	//
	// Overrides.
	//

	virtual void SetViewingContext(const CPresence3<>& pr3);

	virtual SLightInfo ltiGetInfo(const CVector3<>&) const
	{
		return SLightInfo(lvIntensity, d3Dir, angwSize);
	}

	//******************************************************************************************
	virtual void UpdateShadows(CPartition* ppart);
	//
	// Currently, this merely creates the buffer the first time this is called only.  
	// Truly updating it comes later.
	//
	//**********************************

	friend class CLightList;
};


//**********************************************************************************************
//
class CLightPoint: public CLightDir
//
// Prefix: ltp
//
// A light which emanates from a certain point, but is omnidirectional, so that lighting depends
// on distance from the light, but not direction.
//
//**************************
{
public:
	TReal rRadius;			// Extent of the light.
							// At or inside the radius, the illumination is lvIntensity.
							// Outside the radius, the illumination falls off as an
							// inverse square from the light position.

protected:
	CVector3<> v3Position;	// Position of light in transformed space.
	TReal rObjRadius;		// Radius of light in transformed space.

public:

	//******************************************************************************************
	//
	// Constructor.
	//

	CLightPoint
	(
		TLightVal lv_intensity,
		bool b_shadowing = false,
		TReal r_radius = 1
	)
		: CLightDir(lv_intensity, b_shadowing), rRadius(r_radius)
	{
		Assert(r_radius >= 0);
	}

	//******************************************************************************************
	//
	// Overrides.
	//

	virtual bool bIsPositional() const
	{
		return true;
	}

	virtual void SetViewingContext(const CPresence3<>& pr3);

	virtual SLightInfo ltiGetInfo(const CVector3<>& v3) const;
};


//**********************************************************************************************
//
class CLightPointDirectional: public CLightPoint
//
// Prefix: ltpd
//
// The ultimate: a light which emanates from a certain point, and is biased in a certain direction.
// As with CLightDirectional, the direction of emanation is the light's z-axis as transformed by
// its placement.  The variation with direction is determined by two angles, with linear interpolation
// between their cosines.
//
//**************************
{
public:
	TAngleWidth	angwInner;			// Cosine of angle up to which light intensity is maximum.
	TAngleWidth angwOuter;			// Cosine of angle up to which light intensity decreases to zero.

protected:
	CDir3<> d3Direction;			// Direction toward light in transformed space.

public:

	//******************************************************************************************
	//
	// Constructor.
	//

	CLightPointDirectional
	(
		TLightVal lv_intensity, 
		bool b_shadowing = false,
		TReal r_radius		= 1,
		CAngle ang_inner	= dPI / 4, 
		CAngle ang_falloff	= 0
	)
		: CLightPoint(lv_intensity, b_shadowing, r_radius),
		  angwInner(angwFromAngle(ang_inner)), angwOuter(angwFromAngle(ang_inner + ang_falloff))
	{
	}

	//******************************************************************************************
	//
	// Overrides.
	//

	virtual void SetViewingContext(const CPresence3<>& pr3);

	virtual SLightInfo ltiGetInfo(const CVector3<>& v3) const;
};

//**********************************************************************************************
//
class CLightList
//
// Prefix: ltl
//
// A list of lights.  This class contains the cvGetLighting() member.  Thus to actually compute
// lighting, you must place lights in a CLightList.
//
//**************************************
{
protected:
	TLightVal						lvAmbient;		// The total ambient light in the list.
	std::list< rptr<CLightDirectional> >	listpltDir;		// Directional, non-positional lights.
													// Stored in a separate list for optimisation.
	std::list< rptr<CLightDir> >			listpltPos;		// Positional lights.
	std::list< rptr<CLightDir> >			listpltAll;		// Union of above two lists.
	CDir3<>							d3Eye;			// Cached eye direction for scene.

public:
	//******************************************************************************************
	//
	// Constructors.
	//

	CLightList
	(
		const std::list<CInstance*>& listins_lights	// A list of instances whose render types are lights.
	);

	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	bool bIsDirectional() const
	//
	// Returns:
	//		Whether there are any direction-dependent lights.
	//
	//**********************************
	{
		return !listpltAll.empty();
	}

	//******************************************************************************************
	//
	bool bIsPositional() const
	//
	// Returns:
	//		Whether there are any position-dependent lights.
	//
	// Notes:
	//		Used as an optimisation by renderer.
	//
	//**********************************
	{
		return !listpltPos.empty();
	}

	//******************************************************************************************
	//
	void SetViewingContext
	(
		const CPresence3<>& pr3,		// Coordinate space in which to perform calculations.
										// (typically a world-to-object transformation).
		const CPresence3<>& pr3_eye		// Presence of eye relative to pr3.
	);
	//
	// Set the viewing context of self and all children.
	//
	//**********************************

	//******************************************************************************************
	//
	void UpdateShadows
	(
		CPartition* ppart				// Top level of the partition tree to render shadows for,
										// or 0 to remove shadow buffer.
	);
	//
	// Update the shadow buffers of all children.
	//
	//**********************************

	//******************************************************************************************
	//
	TClutVal cvGetLighting
	(
		const CVector3<>& v3,				// The point at which to calculate the light.
		const CDir3<>& d3_normal,			// The normal of the surface at that point.
		const CClut& clut					// Clut specifying material and other properties.
	) const;
	//
	// Returns:
	//		A TClutVal indicating a float index into the clut table.
	//
	//**********************************

	//******************************************************************************************
	//
	SBumpLighting bltGetBumpLighting
	(
		const CMaterial& mat = matDEFAULT	// Material properties.
	) const;
	//
	// Returns:
	//		The info needed for bump mapping a surface.
	//
	// Notes:
	//		There are no arguments for position and normal, because in our current implementation, 
	//		the light for bump-mapping is not allowed to vary in strength or direction across
	//		an object. This is because we have no per-pixel interpolation of light direction,
	//		and per-polygon implementations look bad.
	//
	//**********************************

	//******************************************************************************************
	//
	SBumpLighting bltGetPrimaryBumpLighting() const;
	//
	// Returns:
	//		Info for the primary ambient and directional lights in a scene.
	//		Only the lvAmbient and lvDirectional fields are set.
	//
	//**********************************

};

#endif
