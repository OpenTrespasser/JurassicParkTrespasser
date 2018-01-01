/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Implementation of Material.hpp.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/Material.cpp                                             $
 * 
 * 15    98/10/06 18:48 Speter
 * Fixed silly bug.
 * 
 * 14    98/10/06 16:23 Speter
 * Hack to fix incorrect spec angle data.
 * 
 * 13    8/27/98 1:51p Asouth
 * fully specified the include file (there are two Materials in the DB)
 * 
 * 12    98.08.03 6:40p Mmouni
 * Removed new material message.
 * 
 * 11    98/07/22 14:44 Speter
 * Now contains functions for performing all reflection calculations, independent of clut.
 * Slightly fixed diffuse/specular combining functions.
 * 
 * 10    98.06.30 9:30p Mmouni
 * pmatFindShared() now returns a pointer to matDEFAULT if the requested material is equal to
 * matDEFAULT.
 * 
 * 9     98/04/26 20:16 Speter
 * Cleaned up comments.
 * 
 * 8     4/06/98 3:43p Agrant
 * tsmMaterialInstances now accessible outside of file (for WDBase reset)
 * 
 * 7     2/25/98 6:52p Agrant
 * Printout for a new material definition.
 * 
 * 6     10/24/97 7:31p Agrant
 * Material instancing
 * 
 * 5     97/10/12 21:44 Speter
 * Added lighting functions fDiffuse(), fSpecular(), fReflected().  Changed rvMAX_COLOURED to
 * 1.0, rvMAX_WHITE to 2.0; works better with cluts.
 * 
 * 4     97/08/05 13:29 Speter
 * Changed some default settings.
 * 
 * 3     97/06/23 19:26 Speter
 * Moved const material initialisation to .cpp files.
 * 
 * 2     97/02/13 18:16 Speter
 * Added fAngleFromAngleWidth function.
 * 
 * 1     97/02/13 14:03 Speter
 * Contains implementation of some TAngleWidth functions.
 * 
 **********************************************************************************************/

#include "Common.hpp"
#include "Lib/Renderer/Material.hpp"
#include "Lib/Math/FastTrig.hpp"
#include "Lib/Math/FastSqrt.hpp"

#include "Lib/Sys/DebugConsole.hpp"

#include <math.h>
#include <set.h>
#include <memory.h>


//**********************************************************************************************
//
// TAngleWidth functions.
//

	// The width in radians of the Sun in the sky (radius divided by distance).
	const float fRADIANS_OF_SUN	= 0.05;
	const TAngleWidth	angwDEFAULT_SIZE	= angwFromSin(fRADIANS_OF_SUN);

	//**********************************************************************************************
	TAngleWidth angwFromAngle(float f_angle)
	{
		// Assert for range, but add some tolerance.
		Assert(bWithin(f_angle, 0, dPI_2 * 1.1));
		return cos(f_angle);
	}

	//**********************************************************************************************
	TAngleWidth angwFromSin(float f_sin)
	{
		// Convert the sine to a cosine.
		Assert(bWithin(f_sin, 0, 1));
		return sqrt(1.0 - f_sin * f_sin);
	}

	//**********************************************************************************************
	TAngleWidth angwFromPower(float f_power)
	{
		//
		// Check this funky derivation:
		//
		// We want to find an angw such that the cosine at which intensity is half-maximum
		// is the same as for the power function.  Or:
		//
		//		cos_half_max ^ power = 0.5
		//		cos_half_max = 0.5 ^ (1/power)
		//
		// The angw describes the cosine at which intensity is zero.  Since our angle interpolation
		// is linear with cosine, this cosine is just twice the distance from 1 as cos_half_max:
		//
		//		cos_zero = 1 - 2(1 - cos_half_max)
		//				 = 2 cos_half_max - 1
		//				 = 2 * (0.5 ^ (1/power)) - 1
		//
		// The final wrinkle.  Since the power factor is for use with Phong lighting, and we are
		// doing Blinn lighting, the angle we want is half the one just calculated.  To turn a
		// cosine into the cosine of half the angle, we do this:
		//
		//		cos_half = sqrt((cos_zero + 1)/2)
		//				 = sqrt(0.5 ^ (1/power))
		//				 = 0.5 ^ (0.5/power)
		//

		Assert(f_power > 0);
		return pow(0.5, 0.5 / f_power);
	}

	//**********************************************************************************************
	float fAngleFromAngleWidth(TAngleWidth angw)
	{
		Assert(bWithin(angw, 0, 1));
		return acos(angw);
	}

	/*
		Sample conversions:

		Power	Angle	TAngleWidth (cosine half-angle)
		1		90 d	.7071
		4.4		45 d	.9239
		10		30 d	.9659
		40		15 d	.9914
		91		10 d	.9962
		364		 5 d	.99905
		2275	 2 d	.99985
		9012	 1 d	.99996
	*/

	/*
		(Bump angle resolution is .05 radians; max cosine is .99880; cosine levels = 830)
	*/


//******************************************************************************************
//
//	CMaterial implementation
//

	//**************************************************************************************
	const CMaterial* CMaterial::pmatFindShared
	(
		TReflectVal rv_diffuse,
		TReflectVal rv_specular,
		TAngleWidth angw_specular,
		bool b_reflective,
		bool b_refractive,
		float f_refract_index,
		TReflectVal rv_emissive
	)
	{
		CMaterial mat(rv_diffuse, rv_specular, angw_specular, b_reflective, b_refractive, f_refract_index, rv_emissive);

		// Hack to fix incorrect data. Somehow, the degrees got replaced with the cosine,
		// and amazingly, it happened more than once!  Thus, the while loop.
		if (mat.rvSpecular && mat.angwSpecular != angwZERO)
		{
			float f_degrees;
			while ((f_degrees = fAngleFromAngleWidth(mat.angwSpecular) / dDEGREES) < 1.0f)
				mat.angwSpecular = f_degrees;
		}

		return pmatFindShared(&mat);
	}

	typedef set<CMaterial, less<CMaterial> > TSM;
	TSM tsmMaterialInstances;

	//**************************************************************************************
	const CMaterial* CMaterial::pmatFindShared(const CMaterial *pmat)
	{
		// Specail case for default material, just return a pointer to it.
		// This is necessary so that when &matDEFAULT is used directly
		// it is equivalent to calling pmatFindShared with the default
		// parameters.
		if (*pmat == matDEFAULT)
			return &matDEFAULT;

		// Insert or find, please.
		pair<TSM::iterator, bool> p = tsmMaterialInstances.insert(*pmat);

		// If we found a duplicate, it will do.
		// If we inserted a new one, the new one will do.
		//const CInfo* pinfo_ret = &(*p.first);

		return &(*p.first);
	}

	//**************************************************************************************
	bool CMaterial::operator< (const CMaterial& mat) const
	{
		return 0 > memcmp(this, &mat, sizeof(CMaterial));
	}

	//**************************************************************************************
	bool CMaterial::operator== (const CMaterial& mat) const
	{
		return memcmp(this, &mat, sizeof(CMaterial)) == 0;
	}

	//*****************************************************************************************
	TReflectVal CMaterial::rvSingle(TLightVal lv) const
	{
		Assert(bWithin(lv, 0.0, 1.0));

		if (rvSpecular > rvDiffuse)
		{
			TLightVal lv_spec = fSpecular(lv, angwDEFAULT_SIZE);
			return rvCombined(lv, lv_spec);
		}
		return rvCombined(lv, 0);
	}

	//******************************************************************************************
	float CMaterial::fReflected(float f_cos_in) const
	{
		//
		// Apply the formula giving reflectance for a given angle.  If i is angle of incidence,
		// and r is angle of reflection, then reflectance is:
		//
		//		R = sin²(i-r) / 2 sin²(i+r)  +  tan²(i-r) / 2 tan²(i+r) 
		//		  = (sin²(i-r) / 2 sin²(i+r)) (1 + (1 - sin²(i+r)) / 2 (1 - sin²(i-r)))
		//
		// By Snell's law, if n is the index of refraction:
		//
		//		n(i) sin i = n(r) sin r
		//
		// Therefore,
		//
		//		r = asin (sin i * n(i) / n(r))
		//

		if (!bRefractive)
			return 0.0;

		// No reflection from opposite side.
		if (f_cos_in < 0.0)
			return 0.0;

		if (f_cos_in == 1.0)
		{
			// Return the limit of the formula, which is (n-1)² / (n+1)².
			return Square( (fRefractiveIndex - 1.0) / (fRefractiveIndex + 1.0) );
		}

		float f_angle_in	= acos(f_cos_in);
		float f_sin_out		= sin(f_angle_in) / fRefractiveIndex;

		if (f_sin_out >= 1.0)
			// Total internal reflection; occurs only when passing from slower to faster medium.
			return 1.0;
		else
		{
			float f_angle_out = asin(f_sin_out);
			float f_sin2_m = Square( sin(f_angle_in - f_angle_out) );
			float f_sin2_p = Square( sin(f_angle_in + f_angle_out) );

			return f_sin2_m / f_sin2_p * 
				   (1.0 + (1.0 - f_sin2_p) / (1.0 - f_sin2_m) ) * 
				   0.5;
		}
	}

//**********************************************************************************************
// 
// Global CMaterial definitions.
//

// Default material is same as matte material: no specular highlights.
const CMaterial matMATTE;

// Material with sharp white highlights.
const CMaterial matSHINY(1.0, 1.90, angwFromAngle(0.2)); //lint !e605

// Metal has dull highlights of material colour, and very little diffuse reflection.
const CMaterial matMETAL(0.2, 1.10, angwFromAngle(0.3));  //lint !e605

// Water has max diffuse reflection (because that is used to simulate transmission),
// maximum sharpness, and a refractive index.
const CMaterial matWATER(1.0, 1.80, angwZERO, true, true, 1.33);  //lint !e605

// Mirrored surface has no diffuse reflection, sharp specular, and is reflective.
const CMaterial matMIRROR(0.0, 2.0, angwFromAngle(0), true); 

const CMaterial matDEFAULT;

