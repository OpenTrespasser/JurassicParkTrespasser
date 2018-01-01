/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Math tools fast bumpmapping.
 *
 * Notes:
 *
 * To Do:
 *		Add averaging for increment ranges for the arcsine and arctan lookup tables.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/Primitives/FastBumpMath.hpp                              $
 * 
 * 14    5/06/98 3:32p Rwyatt
 * Adjusted assert in the iArcSin function.
 * 
 * 13    1/19/98 7:30p Pkeet
 * Added support for 16 bit bumpmaps by adding lower colour resolution and a smaller size for
 * 'CBumpAnglePair.'
 * 
 * 12    97/11/24 16:59 Speter
 * Placed all tables and functions inside new CBumpAngleConvert class.  Expanded size of arcsin
 * table.  
 * 
 * 11    97/09/18 21:06 Speter
 * Moved inverse trig functions inline to .hpp.  Changed table sizes.  Moved
 * CInitNormalToBumpAngle class to .cpp, used only there now.
 * 
 * 10    97/08/11 10:12p Pkeet
 * Added the 'dir3AnglesToNormal' function that works directly from the bumpmap angle.
 * 
 * 9     97/03/24 15:23 Speter
 * Now explicitly differentiate PHI_L from PHI_B in constants and usage (rather than just using
 * PHI), allowing them to vary arbitrarily.
 * 
 * 8     96/12/10 13:11 Speter
 * Increased the iArcSin table size to provide full precision for given number of phi angles.
 * 
 * 7     96/11/11 18:54 Speter
 * Changed init class CNormalToBumpAngle to CInitNormalToBumpAngle.
 * Placed static instance of it in .hpp file.
 * 
 * 6     96/09/27 11:31 Speter
 * Oops.  Changed <float> in geometry types to <>.
 * 
 * 5     9/24/96 2:56p Pkeet
 * Added the include for 'FastBump.hpp.' Removed test functions.
 * 
 * 4     9/11/96 11:29a Pkeet
 * Removed '#include "Lib/Renderer/ObjDef3D.hpp".'
 * 
 * 3     9/05/96 6:34p Pkeet
 * Changed bumpmap to use the 'CRasterSimple' class. Added lighting parameters to the
 * 'MakeBumpmap' Table function.
 * 
 * 2     9/04/96 5:16p Pkeet
 * Added file-based bumpmaps.
 * 
 * 1     8/30/96 2:49p Pkeet
 * Initial implementation.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_RENDERER_PRIMITIVES_FASTBUMPMATH_HPP
#define HEADER_LIB_RENDERER_PRIMITIVES_FASTBUMPMATH_HPP


//
// Global includes.
//
#include "FastBump.hpp"


//
// Defines.
//

// Bumpmap angle definitions.
#define iTHETA_PI   (1 << (iTHETA_BITS - 1))
#define iTHETA_PI_2 (1 << (iTHETA_BITS - 2))
#define iTHETA_PI_4 (1 << (iTHETA_BITS - 3))

// Trig table sizes.
#define iARCTAN_TABLE_SIZE_BITS   (iTHETA_BITS + 2)
#define iARCTAN_TABLE_SIZE        (1 << iARCTAN_TABLE_SIZE_BITS)
#define iARCSIN_TABLE_SIZE_BITS   (iPHI_L_BITS + 6)
#define iARCSIN_TABLE_SIZE        (1 << iARCSIN_TABLE_SIZE_BITS)

#define fSHIFT_PHI_BITS   ((float)(1 << (iFLOAT_MANT_BIT_WIDTH - iARCSIN_TABLE_SIZE_BITS + 1)) + 1.0f)
#define fSHIFT_THETA_BITS ((float)(1 << (iFLOAT_MANT_BIT_WIDTH - iARCTAN_TABLE_SIZE_BITS + 1)) + 1.0f)

#if (iTHETA_BITS > 8)
	#error The number of significant bits for bumpmap theta angles must be 8 or less!
#endif

#if (iPHI_L_BITS > 8)
	#error The number of significant bits for bumpmap phi angles must be 8 or less!
#endif


//*********************************************************************************************
//
class CBumpAngleConvert
//
//**************************************
{
protected:
	// Tables for converting normal components to bump angle indices.
	uint8 au1ArcTanTable[iARCTAN_TABLE_SIZE + 1];
	int8  ai1ArcSinTable[iARCSIN_TABLE_SIZE + 1];

	// Tables for looking up angular equivalents to bump phi indices.
	float afPhiBTable[iMAX_PHI_B+1 - iMIN_PHI_B];
	float afPhiLTable[iMAX_PHI_L+1 - iMIN_PHI_L];

public:

	//*********************************************************************************************
	//
	CBumpAngleConvert();
	//
	// Initialise all tables.
	//
	//**********************************

	//*********************************************************************************************
	//
	inline uint uArcTan45
	(
		float f_x_div_y	// Ratio between the x and y components of a 2D vector.
	)
	//
	// Returns the arctan of a value in uTheta units.
	//
	//**************************************
	{
		Assert(bBetween(f_x_div_y, -1.0f, 1.0f));

		CIntFloat if_x_div_y;	// Compound integer and floating point representation of value.

		// Shift the mantissa bits right to convert to a theta tan value.
		if_x_div_y.fFloat  = f_x_div_y + fSHIFT_THETA_BITS;

		// Mask out only valid mantissa bits.
		if_x_div_y.i4Int  &= (iARCTAN_TABLE_SIZE << 1) - 1;

		Assert(if_x_div_y.i4Int <= iARCTAN_TABLE_SIZE);

		// Perform table lookup.
		return au1ArcTanTable[if_x_div_y.i4Int];
	}

	//*********************************************************************************************
	//
	inline int iArcSinL
	(
		float f_z	// Value between -1 and 1.
	)
	//
	// Returns the arcsin of a value in iPhiL units.
	//
	//**************************************
	{
		CIntFloat if_z;	// Compound integer and floating point representation of value.

		// Shift the mantissa bits right to convert to a phi sin value.
		if_z.fFloat  = f_z + fSHIFT_PHI_BITS;

		// Mask out only valid mantissa bits.
		if_z.i4Int  &= (iARCSIN_TABLE_SIZE << 1) - 1;

		Assert(if_z.i4Int <= iARCSIN_TABLE_SIZE);

		// Perform table lookup.
		return ai1ArcSinTable[if_z.i4Int];
	}

	//*********************************************************************************************
	//
	inline int iArcSinB
	(
		float f_z	// Value between -1 and 1.
	)
	//
	// Returns the arcsin of a value in iPhiB units.
	//
	//**************************************
	{
		// We shift the iPhiL value down 0 or more bits to convert to iPhiB.
		Assert(iPHI_L_BITS >= iPHI_B_BITS+1);

		int i_val = iArcSinL(f_z) >> (iPHI_L_BITS-1 - iPHI_B_BITS);
		SetMinMax(i_val, 0, 15);
		return i_val;
	}

	//*********************************************************************************************
	//
	float fGetPhiB
	(
		CBumpAnglePair bang
	)
	//
	//**************************************
	{
		return afPhiBTable[bang.iGetPhiB() - iMIN_PHI_B];
	}

	//*********************************************************************************************
	//
	float fGetPhiL
	(
		CBumpAnglePair bang
	)
	//
	//**************************************
	{
		return afPhiLTable[bang.iGetPhiL() - iMIN_PHI_L];
	}

	//*********************************************************************************************
	//
	CDir3<> dir3AnglesToNormal
	(
		float f_phi,	// Elevation angle in radians.
		float f_theta	// Angle along the xy plane relative to the y axis.
	);
	// 
	// Returns a normalized vector given two angles representing direction and elevation. 
	//
	//**************************************

	//*********************************************************************************************
	//
	CDir3<> dir3AnglesToNormal
	(
		CBumpAnglePair bang
	);
	// 
	// Returns a normalized vector given a bumpmap angle. 
	//
	//**************************************
};

extern CBumpAngleConvert BumpAngleConvert;

#endif
