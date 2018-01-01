/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		TLightVal, TReflectVal
 *		TAngleWidth
 *		CMaterial
 *
 * Bugs:
 *
 * To do:
 *		Change lighting model to incorporate optional coloured lights.
 *		Separate specular lighting from diffuse in TReflectVal.  Combine it in rasteriser.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/Material.hpp                                             $
 * 
 * 34    98/09/28 2:46 Speter
 * Increased limit on annoying assert. Values over 1 don't really hurt, as the clut code is
 * tolerant.
 * 
 * 33    98.08.13 4:26p Mmouni
 * Changes for VC++ 5.0sp3 compatibility.
 * 
 * 32    98/07/22 14:44 Speter
 * Now contains functions for performing all reflection calculations, independent of clut.
 * Slightly fixed diffuse/specular combining functions.
 * 
 * 31    98.06.30 9:27p Mmouni
 * Added 'operator=='.
 * 
 * 30    98/02/26 13:58 Speter
 * Moved TClutVal here from Clut.hpp.
 * 
 * 29    2/23/98 5:11p Agrant
 * Fix the Material instancing bug!
 * 
 * 28    97/12/11 16:39 Speter
 * Now allow rvDiffuse to exceed rvMAX_COLOURED.
 * 
 * 27    10/24/97 7:31p Agrant
 * Material instancing
 * 
 * 26    97/10/12 21:43 Speter
 * Added lighting functions fDiffuse(), fSpecular(), fReflected().  Changed rvMAX_COLOURED to
 * 1.0, rvMAX_WHITE to 2.0; works better with cluts.
 * 
 * 25    97/09/16 15:38 Speter
 * Moved TAngleWidth constants here from RenderDefs.hpp.
 * 
 * 24    97/09/04 18:45 Speter
 * Changed Assert in fCombine to a Fuzzy Assert.
 * 
 * 23    97/06/23 19:26 Speter
 * Moved const material initialisation to .cpp files.
 * 
 * 22    5/26/97 1:37p Agrant
 * LINT mysterious increase in pointer capability errors.
 * Don't understand them, but since we're just declaring some constants
 * it seems safe enough.
 * 
 * 21    97-04-23 14:41 Speter
 * Changed rvCombine to max values rather than using fCombine (works better with specular).
 * 
 * 20    97/02/19 10:34 Speter
 * Moved matWATER here.
 * 
 * 19    97/02/13 18:16 Speter
 * Added fAngleFromAngleWidth function.
 * 
 * 18    97/02/13 14:02 Speter
 * Now contains just the info needed for materials.  Moved TLightVal and TReflectVal here from
 * RenderDefs.hpp, removed RenderDefs.hpp include.  
 * Added TAngleWidth as new specifier of all angular extents, including specular sharpness.
 * Placed in CMaterial def.  Removed fFalloff function.
 * 
 * 17    97/01/26 19:44 Speter
 * Changed name of matPLASTIC to matSHINY (more general).
 * 
 * 16    96/12/17 15:47 Speter
 * Removed all gamma code and comments from lighting code.
 * 
 * 15    96/11/25 13:26 Speter
 * Changed fFalloff function to handle f_factor < 0.  Removed fSpecularFalloff function.
 * Moved matWATER to Water.cpp.
 * 
 * 14    96/11/21 18:56 Speter
 * Added many comments re lighting model.
 * 
 * 13    96/11/14 17:53 Speter
 * Added fields for refractive behaviour, and matWATER material.
 * 
 * 12    96/10/04 17:52 Speter
 * Moved TLightVal, TReflectVal, and SPrimaryReflectData into new RenderDefs.hpp file.
 * 
 * 11    96/09/27 11:32 Speter
 * Oops.  Changed <float> in geometry types to <>.
 * Added some comments.
 * 
 * 10    96/09/23 17:01 Speter
 * Added experimental bReflective field.
 * 
 * 9     96/09/18 19:40 Speter
 * Removed b2Sided field from Material.  We are no longer using 2-sided polygons, and it didn't
 * belong in the material anyway.
 * Added some comments.
 * 
 * 8     96/09/13 14:41 Speter
 * Changed SPrimaryLightData to SPrimaryReflectData; changed members accordingly.
 * 
 * 7     96/09/12 16:20 Speter
 * Added new type TReflectData, differentiated from TLightData.
 * Added SPrimaryLightData structure for bump mapping.
 * 
 * 6     96/09/10 18:21 Speter
 * Added SDirLightData struct, for bump mapping.
 * 
 * 5     96/09/09 18:24 Speter
 * Replaced lvDiffuseLight and lvSpecularLight functions with fSpecularReflection.
 * 
 * 4     96/08/07 20:41 Speter
 * Fixed and improved interaction between CMaterial and CLight.  Now CLight does gamma
 * correction and component combining only on a final sum light value.
 * 
 * 3     96/08/02 11:58 Speter
 * Added Assert and comment.
 * 
 * 2     96/07/24 14:58 Speter
 * Moved code to new Material.cpp file.
 * Added static members bGammaCorrect and fInvMonitorGamma, for dynamic testing.
 * 
 * 
 * 1     96/07/22 15:28 Speter
 * New file, contains stuff moved from Light.hpp
 * 
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_RENDERER_MATERIAL_HPP
#define HEADER_LIB_RENDERER_MATERIAL_HPP


//**********************************************************************************************
//
typedef float	TLightVal;
//
// Prefix: lv
//
// A monochromatic light intensity value, with range [0,1].
// Used to specify strength of a source light, or amount of light incident on a surface.
// See TReflectVal below.
//
//**************************

//**********************************************************************************************
//
typedef float	TReflectVal;
//
// Prefix: rv
//
// A light reflectance value, with range [0,1].
// Used to specify a material reflectance ratio, or a reflected light value.
// The general relationship is that a light's TLightVal is multiplied by a material's TReflectVal
// to yield a TReflectVal.  See comments below.
//
//**************************

#define rvMAX_COLOURED		1.0
#define rvMAX_WHITE			2.0

//
// Lighting colour model.
//
// In reality, an object's "colour" depends on the manner in which light is reflected from it.
// Diffuse reflection might yield one colour, and specular reflection another.  
//
// We use a simpler model that still allows a wide variety of reflectance characteristics: an 
// object has a single "colour", indicated by its underlying texture.  The reflected light 
// colour is determined by the texture colour, and a single scalar light reflectance value.  
// If the light reflectance is between 0 and rvMAX_COLOURED, the reflected colour ranges 
// between black and the object's texture colour.  Between rvMAX_COLOURED and rvMAX_WHITE, 
// the reflected colour ranges between the object's texture colour and white.  
//
// Typically, the object's texture is painted to show maximum diffuse lighting, and the object's 
// diffuse reflectance is rvMAX_COLOURED.  An object's specular reflectance, if any, can be either
// the same colour as its diffuse colour (e.g. metal), in which case it ranges up to rvMAX_COLOURED, 
// or it can be white (e.g. plastic), in which case it ranges up to rvMAX_WHITE.
//
// The object's diffuse and specular reflectance values are combined into a single number by a
// simple max function.  This yields reasonable results, given that white specular reflection
// can be implemented only at the upper end of the colour ramp..
//

//******************************************************************************************
//
inline float fCombine
(
	float f_a, float f_b,			// Two values between 0 and f_max.
	float f_max = 1.0				// The maximum range of the two.
)
//
// Returns:
//		A combined value between 0 and f_max.
//
// Notes:
//		This function is similar to addition, but ensures that the result remains between
//		0 and f_max.  It uses the formula:
//
//			result = a + b - a*b/f_max
//
//		This function is used for combining lighting values, allowing an arbitrary number
//		of lights to be specified in a scene without worrying about overflow.  The clamping
//		via this method is "softer" than simple clamping.
//
//**********************************
{
	Assert(Fuzzy(f_a, .001f).bWithin(0, f_max));
	Assert(Fuzzy(f_b, .001f).bWithin(0, f_max));

	return f_a + f_b - f_a * f_b / f_max;
}

//******************************************************************************************
//
inline TReflectVal rvCombine
(
	TReflectVal rv_colour,
	TReflectVal rv_hilite
)
//
// Returns:
//		A TReflectVal combining the two values.
//
//**************************************
{
	return Max(rv_colour, rv_hilite);
}

//*********************************************************************************************
//
typedef float TClutVal;
//
// Prefix: cv
//
// Denotes a floating-point value providing index into clut.
//
//**************************************

//**********************************************************************************************
//
typedef float TAngleWidth;				
//
// Prefix: angw
//
// Represents an angular extent in a number of contexts: the physical radius of a light in the sky,
// the radius of a point directional light's angular dispersion, and the radius of specular
// reflection.  Note that the *radius* of all these widths is specified, not the diameter.
//
// Stored as the cosine of the angle, because that allows efficient comparisons with normal
// dot products.  
//
//**************************************

//**********************************************************************************************
//
// TAngleWidth functions.
//

	//**********************************************************************************************
	//
	TAngleWidth angwFromAngle
	(
		float f_angle					// The angular width to convert.
	);
	//
	// Returns:
	//		The corresponding TAngleWidth.
	//
	//**********************************

	//**********************************************************************************************
	//
	TAngleWidth angwFromSin
	(
		float f_sin						// The sin of the angle to convert.
	);
	//
	// Returns:
	//		The corresponding TAngleWidth.
	// 
	// Notes:
	//		Useful for creating a TAngleWidth for the angular size of an object.
	//		The ratio of the object's size to its distance is f_sin.
	//
	//**********************************

	//**********************************************************************************************
	//
	TAngleWidth angwFromPower
	(
		float	f_power					// The Phong specular power to convert.
	);
	//
	// Returns:
	//		The corresponding TAngleWidth.
	//
	//**********************************


	//******************************************************************************************
	//
	inline float fAngularStrength
	(
		float f_cos,					// The cosine of the angle with specular.
		TAngleWidth angw_inner,			// The inner (full-strength) cone angle.
		TAngleWidth angw_outer			// The outer (zero-strength) cone angle.
	)
	//
	// Returns:
	//		The relative lighting strength for the given cosine, linearly interpolated between
	//		the two angle widths.
	//
	//**********************************
	{
		Assert(bWithin(angw_inner, 0, 1));
		Assert(bWithin(angw_outer, 0, angw_inner));

		if (f_cos >= angw_inner)
			// It's within the full-strength cone.
			return 1;
		else if (f_cos < angw_outer)
			// It's outside the zero-strength cone.
			return 0;
		else
			// Linearly interpolate.
			return (f_cos - angw_outer) / (angw_inner - angw_outer);
	}

	//**********************************************************************************************
	//
	float fAngleFromAngleWidth
	(
		TAngleWidth angw				// An angle width to convert.
	);
	//
	// Returns:
	//		The corresponding angle, in radians.
	//
	//**********************************

//**********************************************************************************************
//
// TAngleWidth constants.
//

	const			TAngleWidth angwZERO = 1;			// Angle is 0, so cosine is 1.
	extern const	TAngleWidth	angwDEFAULT_SIZE;		// The default light size.

//**********************************************************************************************
//
class CMaterial
//
// Prefix: mat
//
// Describes the lighting properties of an object.
//
//**************************************
{
public:
	TReflectVal	rvDiffuse;				// Intensity of diffuse (and ambient) reflections.
	TReflectVal	rvSpecular;				// Intensity of specular reflections.
	TAngleWidth	angwSpecular;			// Angle width defining sharpness of specular reflections.

	// These need to be bigger so that all of the bits are used.  Otherwise, memcmp will reveal differences in the hidden
	// bits.  Since the rest of the fields are DWORD aligned anyway, we don't lose anything.
	int			bReflective: 16,		// Material reflects the environment.
				bRefractive: 16;		// Material refracts (and reflects).
	float		fRefractiveIndex;		// Index of refraction of the material.
	TReflectVal	rvEmissive;				// Intensity of self-luminous light.

public:

	CMaterial
	(
		TReflectVal rv_diffuse	= rvMAX_COLOURED, 
		TReflectVal rv_specular	= 0,
		TAngleWidth angw_specular = angwZERO,
		bool b_reflective		= false,
		bool b_refractive		= false,
		float f_refract_index	= 1.0,
		TReflectVal rv_emissive	= 0
	)
	{
		rvDiffuse		= rv_diffuse;
		rvSpecular		= rv_specular;
		angwSpecular	= angw_specular;
		bReflective		= b_reflective;
		bRefractive		= b_refractive;
		fRefractiveIndex= f_refract_index;
		rvEmissive		= rv_emissive;

		Assert(bWithin(rv_diffuse,	0, rvMAX_WHITE));
		Assert(bWithin(rv_specular,	0, rvMAX_WHITE));
		Assert(bWithin(rv_emissive,	0, rvMAX_WHITE));
		Assert(bWithin(angw_specular, 0, 1));
		Assert(f_refract_index >= 1.0);
	}


	static const CMaterial* pmatFindShared
	(
		TReflectVal rv_diffuse	= rvMAX_COLOURED, 
		TReflectVal rv_specular	= 0,
		TAngleWidth angw_specular = angwZERO,
		bool b_reflective		= false,
		bool b_refractive		= false,
		float f_refract_index	= 1.0,
		TReflectVal rv_emissive	= 0
	);

	static const CMaterial* pmatFindShared
	(
		const CMaterial *pmat
	);

	bool operator<(const CMaterial& mat) const;

	bool operator==(const CMaterial& mat) const;

	//**********************************************************************************************
	//
	float fDiffuse
	(
		float f_cosine_in				// The cosine of the angle between the surface and
										// the incident light.
	) const
	//
	// Returns:
	//		The proportion of light diffusely reflected at this angle.
	//
	//**********************************
	{
		return f_cosine_in;
	}

	//**********************************************************************************************
	//
	float fSpecular
	(
		float f_cosine_in,				// The cosine of the angle between the surface and
										// the maximum specular highlight direction.
		TAngleWidth angw_light_size		// Angular size of the light.
	) const
	//
	// Returns:
	//		The proportion of light specularly reflected at this angle.
	//
	// Applies only to materials that have rvSpecular > 0.
	//
	//**********************************
	{
		if (rvSpecular <= rvDiffuse)
			return 0.0;

		//
		// The outer angle is the sum of the light_size and specular angles.
		// Approximate the cosine of the angle sums by multiplying the cosines.
		//
		TAngleWidth angw_inner = angwSpecular * angw_light_size;
		return fAngularStrength(f_cosine_in, angw_light_size, angw_inner);
	}

	//**********************************************************************************************
	//
	float fReflected
	(
		float f_cosine_in				// The cosine of the angle between the surface and
										// the viewer.
	) const;
	//
	// Returns:
	//		The proportion of light reflected at this angle.
	//
	// Applies only to materials that are bRefractive.
	//
	//**********************************

	//**********************************************************************************************
	//
	TReflectVal rvCombined
	(
		TLightVal lv_diffuse,			// Incident lighting producing diffuse reflection.
		TLightVal lv_specular			// Incident lighting producing specular reflection.
	) const
	//
	// Returns:
	//		The total reflection value on this material for the given lighting.
	//
	//**********************************
	{
		Assert(bWithin(lv_diffuse, 0.0, 1.001));
		Assert(bWithin(lv_specular, 0.0, 1.001));

		// Specular intensity used to fade from diffuse reflectance up to
		// maximum specular reflectance.
		return	lv_diffuse * rvDiffuse * (1.0 - lv_specular) +
				rvSpecular * lv_specular;
	}

	//*****************************************************************************************
	//
	TReflectVal rvSingle
	(
		TLightVal lv					// Incident light value from composite light.
	) const;
	//
	// Returns:
	//		The total reflection value on this material for the given combined lighting.
	//
	//**********************************
};

//**********************************************************************************************
//
// CMaterial constants.
//

// Default material is same as matte material: no specular highlights.
extern const CMaterial matDEFAULT;
extern const CMaterial matMATTE;

// Material with sharp white highlights.
extern const CMaterial matSHINY;

// Metal has dull highlights of material colour, and very little diffuse reflection.
extern const CMaterial matMETAL;

// Water has max diffuse reflection (because that is used to simulate transmission),
// maximum sharpness, and a refractive index.
extern const CMaterial matWATER;

// Mirrored surface has no diffuse reflection, sharp specular, and is reflective.
extern const CMaterial matMIRROR;

#endif
