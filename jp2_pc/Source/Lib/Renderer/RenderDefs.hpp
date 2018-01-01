/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Render definitions common to 2D and 3D modules.
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/RenderDefs.hpp                                           $
 * 
 * 24    8/25/98 11:42a Rvande
 * Fully specified path to filename which exists in multiple directories
 * 
 * 23    7/29/98 10:49a Rwyatt
 * Added a render flag for textures that have had mip maps generated.
 * 
 * 22    7/28/98 8:25p Pkeet
 * Added tiling flags.
 * 
 * 21    3/27/98 5:35p Pkeet
 * Added the 'erfD3D_CACHE' flag.
 * 
 * 20    98/03/02 12:11 Speter
 * Added version number for ERenderFeature.
 * 
 * 19    98/02/26 15:38 Speter
 * Removed erfALPHA_SHADE and erfALPHA_TEXTURE. Added erfCURVED for textures.
 * 
 * 18    98/02/26 13:55 Speter
 * Removed no-longer needed include.
 * 
 * 17    97/10/12 21:54 Speter
 * Removed pMaterial from SLightInfo (contained in texture).
 * 
 * 16    97/10/01 4:39p Pkeet
 * Added the 'erfOCCLUDE' flag.
 * 
 * 15    97/09/16 15:41 Speter
 * Made SBumpLighting structure much simpler, incorporating a SLightInfo for combined
 * diffuse/specular lighting, and a pointer to material.  Removed SDirReflectData and
 * SPrimaryReflectData structures.  
 * 
 * 14    97/08/17 4:15p Pkeet
 * Added the 'erfSOURCE_WATER' flag.
 * 
 * 13    97/08/09 1:56p Pkeet
 * Added flags for alpha blending.
 * 
 * 12    97/08/08 11:51a Pkeet
 * Added 'erfMIPMAP.'
 * 
 * 11    97/07/28 11:13a Pkeet
 * Added the 'erfSOURCE_TERRAIN' and 'erfDRAW_CLIP' settings.
 * 
 * 10    97/06/27 15:35 Speter
 * Added erfTRAPEZOIDS feature.
 * 
 * 9     97/06/25 12:46p Pkeet
 * Support for a wireframe mode.
 * 
 * 8     97-05-08 13:17 Speter
 * Slightly sped up SDirReflection constructor (speeds up pipeline!).  Moved initialisation of
 * SBumpLighting member from SRenderTriangle, etc. to SBumpLighting constructor.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_RENDERER_RENDERDEFS_HPP
#define HEADER_LIB_RENDERER_RENDERDEFS_HPP

#include "Lib/Renderer/Material.hpp"
#include "Lib/Transform/Vector.hpp"

enum ERenderFeature
// Enumeration of drawing primitive flags.
// These are combined into a set with CSet<ERenderFeature>.
// These form a superset of features available in all known CScreenRender rasterisers.
// Prefix: erf
{
	//
	// Features for rendering primitive.
	//
	// Shading.
	erfCOPY,			// Copy colours to polygon, no colour translation (implies no lighting).
	erfLIGHT_SHADE,		// Interpolate lighting across polygon (implies erfLIGHT).

	// Textures.
	erfTEXTURE,			// Apply textures to polygons.
	erfTRANSPARENT,		// The textures have transparent pixel 0 (implies erfTEXTURE).
	erfBUMP,			// Apply bump mapping to polygons (implies erfTEXTURE).
	erfDRAW_CLIP,		// The polygon will overwrite pixels with transparent pixels.
	erfALPHA_COLOUR,	// The source is to be alpha-blended with the dest.
						// If erfTEXTURE is also set, it's an alpha texture.
						// If erfLIGHT_SHADE is also set, it's interpolated alpha.
						// If neither is set, it's a solid alpha colour.
	erfMIPS_CREATED,	// Set on a texture when the mips have been generated

	// Pixels.
	erfPERSPECTIVE,		// Texture-mapping etc. is perspective-correct.
	erfFILTER,			// Pixel filtering internal to textures.
	erfFILTER_EDGES,	// Pixel filtering on texture edges (anti-aliasing).
	erfSUBPIXEL,		// Texture-mapping etc. is subpixel-correct.
	erfDITHER,			// Dither to simulate higher-resolution colours.

	erfZ_BUFFER,		// Full Z-buffering.
//	erfZ_WRITE,			// Write to Z buffer, without testing it.
//	erfZ_TEST,			// Test Z buffer, but don't write it.

	//
	// Features for setup.
	//
	erfTRAPEZOIDS,		// Rasterise with trapezoids when able.
	erfRASTER_CLIP,		// Rasteriser will clip polygon primitives.
	erfRASTER_CULL,		// Rasteriser will backface cull the primitives.

	//
	// Lighting.
	//
	erfLIGHT,			// Apply the polygon lighting values to the polygon colour.
	erfFOG,				// Apply the polygon fogging values to the polygon colour.
	erfFOG_SHADE,		// Interpolate fogging values across polygon (implies erfFOG).
	erfSPECULAR,		// Specular highlights are calculated and rendered.
	erfCOLOURED_LIGHTS,	// All lighting values are full RGB, rather than greyscale.
	erfWIRE,			// Use wireframe mode.

	//
	// Source.
	//
	erfSOURCE_TERRAIN,	// Indicates that the polygon originates with the terrain mesh.
	erfSOURCE_WATER,	// Indicates that the polygon originates with a water mesh.
	erfOCCLUDE,			// Indicates that the polygon is an occluding polygon only.
	erfMIPMAP,			// Indicates that a polygon may be mipmapped.
	erfCURVED,			// Indicates that the polygon has had curvature applied to its bump-map.
	erfD3D_CACHE,		// Direct3D image cache.

	//
	// Tiling.
	//
	erfTILE_UV,			// Indicates that the polygon tiles in u and v.
	erfTILE_U,			// Indicates that the polygon tiles in u only.
	erfTILE_V,			// Indicates that the polygon tiles in v only.

	//
	// Version number.
	// This should not be combined into a render flag set, but is used to validate numerical
	// render flag sets read from an options file.
	// Be sure to increment this any time you add/change ERenderFeature.
	//
	erfVERSION = 4
};

//**********************************************************************************************
struct SLightInfo
//
// Prefix: lti
//
// Describes a light strength and direction at a particular point in space.
//**************************************
{
	TLightVal	lvStrength;				// Intensity of light.
	CDir3<>		d3Light;				// Direction of light.
	TAngleWidth	angwSize;				// The cosine of the light's angular size.

	SLightInfo(TLightVal lv_strength = 0)
		: lvStrength(lv_strength), angwSize(1)
	{
	}

	SLightInfo(TLightVal lv_strength, const CDir3<>& d3, TAngleWidth angw = angwDEFAULT_SIZE)
		: lvStrength(lv_strength), d3Light(d3), angwSize(angw)
	{
	}
};

//**********************************************************************************************
struct SBumpLighting: SLightInfo
// Prefix: blt
// Data needed for bump-mapping.
{
	TLightVal				lvAmbient;		// The ambient light in a scene.

	SBumpLighting()
		: lvAmbient(0)
	{
	}

	SBumpLighting
	(
		TLightVal lv_ambient, 
		TLightVal lv_strength, 
		const CDir3<>& d3, 
		TAngleWidth angw = angwDEFAULT_SIZE
	) :
		SLightInfo(lv_strength, d3, angw), lvAmbient(lv_ambient)
	{
	}
};

#endif
