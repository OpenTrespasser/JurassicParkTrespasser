/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Implementation of Colour.hpp.
 *
 * Bugs:
 *
 * To do:
 *		Replace i_dummy parameter in CColour(uint32) with "explicit" keyword when available.
 *		Make a derived class that deals with u1Flags.
 *		Implement iMatchSimple as an inline assembly function.
 *		Replace byte scaling multiplication with a fast lookup table in assembly.
 *		Figure out how many light shading levels we will really support (32?), and
 *		incorporate that into scaling routines and lookup tables.
 *		Add HSV etc. conversions.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/View/Colour.cpp                                                   $
 * 
 * 6     9/22/98 10:39p Pkeet
 * Forced inline critical Direct3D functions.
 * 
 * 5     8/20/98 5:38p Mmouni
 * Changes for additional clut control text props.
 * 
 * 4     98/07/20 12:08 Speter
 * Moved some floating point functions to .cpp file.
 * 
 * 3     97/10/02 12:08 Speter
 * Changed iFloatCastto faster iPosFloatCast().
 * 
 * 2     6/25/97 7:26p Mlange
 * Now uses the (fast) float to integer conversion functions.
 * 
 * 1     97/06/03 18:12 Speter
 * Moved inline functions from Colour.hpp here.  Commented out unused ones.
 * 
 **********************************************************************************************/

#include "common.hpp"
#include "Colour.hpp"

#include "Lib/Math/FloatDef.hpp"

//******************************************************************************************
//
// CColour implementation.
//

	//
	// Constants.
	//

	const float fRedScale	= 0.3;
	const float fGreenScale	= 0.6;
	const float fBlueScale	= 0.1;

	//******************************************************************************************
	CColour::CColour(double d_red, double d_green, double d_blue)
	{
		Assert(bWithin(d_red,   0.0, 1.0) &&
			   bWithin(d_green, 0.0, 1.0) &&
			   bWithin(d_blue,  0.0, 1.0));
		
		u1Red	= iRound(d_red   * 255.0);
		u1Green	= iRound(d_green * 255.0);
		u1Blue	= iRound(d_blue  * 255.0);
		u1Flags	= 0;
	}

	//******************************************************************************************
	float CColour::fIntensity() const
	{
		return u1Red   * (fRedScale   / 255.0) +
			   u1Green * (fGreenScale / 255.0) +
			   u1Blue  * (fBlueScale  / 255.0);
	}

	//******************************************************************************************
	uint8 CColour::u1Intensity() const
	{
		return iRound
		(
			u1Red   * fRedScale   +
			u1Green * fGreenScale +
			u1Blue  * fBlueScale
		);
	}

	//******************************************************************************************
	CColour CColour::operator *(float f_scale) const
	{
		Assert(bWithin(f_scale, 0.0f, 1.0f));

		return CColour
		(
			iRound(u1Red   * f_scale),
			iRound(u1Green * f_scale),
			iRound(u1Blue  * f_scale)
		);
	}


//
// Global functions.
//

//******************************************************************************************
int iDifference(CColour clr_a, CColour clr_b)
{
	// Colour bias`s, as per CIE recommendations, there are a couple of sets of these , so maybe i should include them.
	const float fRED_BIAS   =0.299f;
	const float fGREEN_BIAS =0.587f;
	const float fBLUE_BIAS  =0.114f;

	float f_calc_a, f_calc_b, f_bias_diff;
	int i_r_diff, i_g_diff, i_b_diff;

	// Calc bias`d colours.
	f_calc_a = ((fRED_BIAS * clr_a.u1Red) + (fGREEN_BIAS * clr_a.u1Green) + (fBLUE_BIAS * clr_a.u1Blue));
	f_calc_b = ((fRED_BIAS * clr_b.u1Red) + (fGREEN_BIAS * clr_b.u1Green) + (fBLUE_BIAS * clr_b.u1Blue));

	// Difference between non biased componets.
	i_r_diff = ((clr_a.u1Red   - clr_b.u1Red  ) * (clr_a.u1Red   - clr_b.u1Red  ));
	i_g_diff = ((clr_a.u1Green - clr_b.u1Green) * (clr_a.u1Green - clr_b.u1Green));
	i_b_diff = ((clr_a.u1Blue  - clr_b.u1Blue ) * (clr_a.u1Blue  - clr_b.u1Blue ));

	// Diff between biased colours.
	f_bias_diff = ((f_calc_a - f_calc_b) * (f_calc_a - f_calc_b));

	// Return sum of all diferences.
	return iPosFloatCast(f_bias_diff + i_r_diff + i_g_diff + i_b_diff);
};



#if 0
// Following are not currently used.

//******************************************************************************************
inline float fMAX(float r,float g,float b)
{
	float fTemp= ( float )0.0;

	if(r>fTemp) fTemp=r;
	if(g>fTemp) fTemp=g;
	if(b>fTemp) fTemp=b;
	return fTemp;
}

//******************************************************************************************
inline float fMIN(float r,float g,float b)
{
	float fTemp = (float)3000.0;

	if(r<fTemp) fTemp=r;
	if(g<fTemp) fTemp=g;
	if(b<fTemp) fTemp=b;
	return fTemp;
}

#define UNDEFINED (float)-1.0

//******************************************************************************************
void rgb2hsv(float fR,float fG,float fB,float *fH,float *fS,float *fV)
{
/*
 Given: r,g,b , each in [0,1]
 Desired: h in [0,360],s and v in [0,1] except if s = 0, then h =
 UNDEFINED, which is some constant defined with a value outside
 the interval [0.360] */

 	float fMin,fMax,fDelta;

	fMax = fMAX(fR,fG,fB);
	fMin = fMIN(fR,fG,fB);

	*fV = fMax;		/* This is the value v */

	/* next calculate saturation s */

	if(fMax != (float)0.0)
		*fS= ( fMax-fMin )/fMax;
	else
		*fS = (float)0.0;	/* saturation is 0 if if rgb are all 0 */

	if(*fS == (float)0.0) {
		*fH = UNDEFINED;
		return;
	}

	/* Chromatic case: Saturation is not 0, so determine hue */

	fDelta = fMax - fMin;

	if( fR==fMax ) 		/* resulting */
		*fH=( fG-fB )/fDelta;
	else if ( fG  == fMax )
	 *fH = (float)2.0+( fB-fR )/fDelta;
	else if ( fB == fMax )
		*fH = (float)4.0 + ( fR-fG )/fDelta;

	*fH *= (float)60.0;			/* convert hue to degrees */

	if(*fH<(float)0.0)
		*fH+=(float)360.0;	/* make sure hue is non-negative */
}

//******************************************************************************************
void hsv2rgb(float*fR,float*fG,float *fB,float fH,float fS,float fV)
{
/* Given: h in [0,360] or UNDEFINED, s amd v in [0,1].
	Desired: r,g,b, each in [0,1] */

	float fF,fP,fQ,fT;
	long iI;

	if(fS==(float)0.0) {
		if(fH != UNDEFINED)
			return; /* invalid value */

		*fR=fV;
		*fG=fV;
		*fB=fV;
		return;
	}

	if(fH == (float)360.0)
		fH = (float)0.0;

	fH /=(float)60.0;
	iI = (long)floor(fH);
	fF = fH-iI;
	fP = fV * (1-fS);
	fQ = fV * (1-fS*fF);
	fT = fV * (1-fS*(1-fF));

	switch(iI)
	{
		case 0:
			*fR = fV;
			*fG = fT;
			*fB = fP;
			break;
		case 1:
			*fR = fQ;
			*fG = fV;
			*fB = fP;
			break;
		case 2:
			*fR = fP;
			*fG = fV;
			*fB = fT;
			break;
		case 3:
			*fR = fP;
			*fG = fQ;
			*fB = fV;
			break;
		case 4:
			*fR = fT;
			*fG = fP;
			*fB = fV;
			break;
		case 5:
			*fR = fV;
			*fG = fP;
			*fB = fQ;
			break;
 	}
}

#endif
