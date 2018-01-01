/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Implementation of 'FastBumpMath.hpp.'
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/Primitives/FastBumpMath.cpp                              $
 * 
 * 22    8/27/98 9:12p Asouth
 * loop variable moved out of loop scope
 * 
 * 21    1/19/98 7:30p Pkeet
 * Added support for 16 bit bumpmaps by adding lower colour resolution and a smaller size for
 * 'CBumpAnglePair.'
 * 
 * 20    97/11/25 19:27 Speter
 * Fixed bug.  Tables were initialised in wrong order, causing incorrect MIP levels, etc.
 * 
 * 19    97/11/24 16:59 Speter
 * Placed all tables and functions inside new CBumpAngleConvert class.  Expanded size of arcsin
 * table.  Added flag to alter distrubution of phi angles (turned off for now, since it doesn't
 * work well with water).
 * 
 * 18    97/10/02 12:11 Speter
 * Changed iFloatCast() to more correct iRound() in inv trig tables.
 * 
 * 17    97/09/18 21:08 Speter
 * Moved inverse trig functions inline to .hpp.  Moved CInitNormalToBumpAngle class to .cpp,
 * used only there now.  Implemented init with #pragma init_seg().  Changed av3AngleToNormal to
 * ad3AngleToNormal, no longer has init conflict.  Removed old code.
 * 
 * 16    8/12/97 8:35p Pkeet
 * Fixed bug.
 * 
 * 15    97/08/11 10:12p Pkeet
 * Added the 'av3AngleToNormal' table and the 'dir3AnglesToNormal' function that works directly
 * from the bumpmap angle.
 * 
 * 14    6/25/97 7:27p Mlange
 * Now uses the (fast) float to integer conversion functions.
 * 
 * 13    97/06/03 18:49 Speter
 * Added #include FloatDef.hpp.
 * 
 * 12    97/03/24 15:23 Speter
 * Now explicitly differentiate PHI_L from PHI_B in constants and usage (rather than just using
 * PHI), allowing them to vary arbitrarily.
 * 
 * 11    96/11/27 19:38 Speter
 * Changed float-to-int angle conversion to use rounding.
 * 
 * 10    96/11/11 18:54 Speter
 * Changed init class CNormalToBumpAngle to CInitNormalToBumpAngle.
 * Placed static instance of it in .hpp file.
 * 
 * 9     96/10/04 18:03 Speter
 * Removed include of defunct ObjDef3D.hpp.
 * 
 * 8     10/01/96 3:15p Pkeet
 * Added faster angle to normal conversion.
 * 
 * 7     96/09/27 11:31 Speter
 * Oops.  Changed <float> in geometry types to <>.
 * 
 * 6     96/09/25 19:50 Speter
 * The Big Change.
 * In transforms, replaced TReal with TR, TDefReal with TReal.
 * Replaced all references to transform templates that have <TObjReal> with <>.
 * Replaced TObjReal with TReal, and "or" prefix with "r".
 * Replaced CObjPoint, CObjNormal, and CObjPlacement with equivalent transform types, and
 * prefixes likewise.
 * Removed some unnecessary casts to TReal.
 * Finally, replaced VER_GENERAL_DEBUG_ASSERTS with VER_DEBUG.
 * 
 * 5     9/24/96 2:59p Pkeet
 * Added comments and removed unnecessary includes and functions.
 * 
 * 4     9/11/96 11:31a Pkeet
 * Added 'fPhiError' and 'fThetaError,' also added the 'ObjDef3D' include.
 * 
 * 3     9/05/96 6:34p Pkeet
 * Changed bumpmap to use the 'CRasterSimple' class. Added lighting parameters to the
 * 'MakeBumpmap' Table function.
 * 
 * 2     9/04/96 5:16p Pkeet
 * Added file-based bumpmaps.
 * 
 * 1     8/30/96 2:56p Pkeet
 * Initial implementation.
 * 
 **********************************************************************************************/


//
// Includes.
//
#include "Common.hpp"
#include "FastBumpMath.hpp"

#include "Lib/Math/FloatDef.hpp"

// Give this module run-time init priority.
#pragma warning(disable: 4073)
#pragma init_seg(lib)

// Use a table lookup for fast bumpmap angle to normal conversions.
#define bUSE_TABLE_FOR_ANGLE_TO_NORMAL (1)

// Quantize the phi angles in units proportional to cosine, not angle.
#define bSTORE_PHI_COS		(0)

//
// Module specific data.
//
#if bUSE_TABLE_FOR_ANGLE_TO_NORMAL

	#define iANGLE_TO_NORMAL_MASK (iMASK_THETA | iMASK_PHI_B)
	#define iANGLE_TO_NORMAL_SIZE (iANGLE_TO_NORMAL_MASK + 1)

	static CDir3<> ad3AngleToNormal[iANGLE_TO_NORMAL_SIZE];

#endif // bUSE_TABLE_FOR_ANGLE_TO_NORMAL

static bool bInit = false;

//*********************************************************************************************
//
// class CBumpAngleConvert implementation.
//

	//*****************************************************************************************
	CBumpAngleConvert::CBumpAngleConvert()
	{
		Assert(!bInit);
		bInit = true;

		const float fPHI_B_SCALE = 0.9999 / float(iMAX_PHI_B);
		const float fPHI_L_SCALE = 0.9999 / float(iMAX_PHI_L);

		int i;
		for (i = iMIN_PHI_B; i <= iMAX_PHI_B; i++)
		{
#if bSTORE_PHI_COS
			afPhiBTable[i - iMIN_PHI_B] = acos(1.0 - float(i) * fPHI_B_SCALE);
#else
			afPhiBTable[i - iMIN_PHI_B] = float(i) * float(dPI_2) * fPHI_B_SCALE;
#endif
		}

		for (i = iMIN_PHI_L; i <= iMAX_PHI_L; i++)
		{
#if bSTORE_PHI_COS
			if (i < 0)
				afPhiLTable[i - iMIN_PHI_L] = - acos(1.0 - float(-i) * fPHI_L_SCALE);
			else
				afPhiLTable[i - iMIN_PHI_L] = acos(1.0 - float(i) * fPHI_L_SCALE);
#else
			afPhiLTable[i - iMIN_PHI_L] = float(i) * float(dPI_2) * fPHI_L_SCALE;
#endif
		}

		int   i_z,   i_x_div_y;	// Integer representations of arc-trig functions.
		float f_z,   f_x_div_y;	// Floating point representations of arc-trig functions.
		float f_phi, f_theta;	// Floating point representations of angles.

		//
		// Make the arcsin table.
		// Use PHI_L for range, as it may be higher res than PHI_B.
		//
		Assert(iMAX_PHI_L >= iMAX_PHI_B);

		for (i_z = 0; i_z < iARCSIN_TABLE_SIZE; i_z++)
		{
			// Convert the index of the table to a valid arcsine value in the range -1 to 1.
			f_z = 2.0f * (float(i_z) / float(iARCSIN_TABLE_SIZE)) - 1.0f;

#if bSTORE_PHI_COS
			f_phi = (1.0 - sqrt(1.0 - f_z * f_z)) * float(iMAX_PHI_L);
			if (f_z < 0.0)
				f_phi = -f_phi;
#else		
			// Get the angle in radians from the arcsine.
			f_phi = float(asin(f_z)) * float(iMAX_PHI_L) / dPI_2;
#endif

			// Convert the angle in radians to the angle in phi units and add to table.
			ai1ArcSinTable[i_z]  = iRound(f_phi);
		}

		//
		// The last element of the table must be entered manually; this value corresponds to
		// arcsin(1).
		//
		ai1ArcSinTable[iARCSIN_TABLE_SIZE] = iMAX_PHI_L;

		//
		// Make the arctan table for -45 degrees to +45 degrees.
		//
		for (i_x_div_y = 0; i_x_div_y < iARCTAN_TABLE_SIZE; i_x_div_y++)
		{
			// Convert the index of the table to a valid arctan value in the range -1 to 1.
			f_x_div_y                 = 2.0f * (float(i_x_div_y) / float(iARCTAN_TABLE_SIZE)) - 1.0f;

			// Get the angle in radians from the arctan.
			f_theta = float(atan(f_x_div_y));

			// Convert the angle in radians to the angle in theta units and add to table.
			f_theta                  *= float(1 << iTHETA_BITS) / d2_PI;
			au1ArcTanTable[i_x_div_y]  = iRound(f_theta);
		}

		//
		// The last element of the table must be entered manually; this value corresponds to
		// arctan(1).
		//
		au1ArcTanTable[iARCTAN_TABLE_SIZE] = iTHETA_PI_4;

		//
		// Build the bump angle to normal lookup table.
		//
	#if bUSE_TABLE_FOR_ANGLE_TO_NORMAL

		CBumpAnglePair bang;
		for (bang.br = 0; bang.br < iANGLE_TO_NORMAL_SIZE; ++bang.br)
		{
			ad3AngleToNormal[bang.br] = dir3AnglesToNormal(bang.fGetPhiB(), bang.fGetTheta());
		}

	#endif // bUSE_TABLE_FOR_ANGLE_TO_NORMAL
	}

	//*********************************************************************************************
	CDir3<> CBumpAngleConvert::dir3AnglesToNormal(float f_phi, float f_theta)
	{
		CDir3<> d3;

		//
		// Create a vector extending out in the y axis. Note that this is because the angle theta
		// is relative to y.
		//

		// Direct conversion of angles to a normalized vector.
		float f_cos_phi = float(cos(f_phi));

		d3.tX = -float(sin(f_theta)) * f_cos_phi;
		d3.tY =  float(cos(f_theta)) * f_cos_phi;
		d3.tZ =  float(sin(f_phi));

		return d3;
	}

	//*********************************************************************************************
	CDir3<> CBumpAngleConvert::dir3AnglesToNormal(CBumpAnglePair bang)
	{
		#if bUSE_TABLE_FOR_ANGLE_TO_NORMAL
			Assert(bInit);
			return ad3AngleToNormal[bang.br & iANGLE_TO_NORMAL_MASK];
		#else
			return dir3AnglesToNormal(bang.fGetPhiB(), bang.fGetTheta());
		#endif // bUSE_TABLE_FOR_ANGLE_TO_NORMAL
	}


//
// Global Variables.
//

CBumpAngleConvert BumpAngleConvert;
