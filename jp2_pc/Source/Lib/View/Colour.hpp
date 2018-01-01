/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		class CColour
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/View/Colour.hpp                                                   $
 * 
 * 28    9/22/98 10:39p Pkeet
 * Forced inline critical Direct3D functions.
 * 
 * 27    7/26/98 7:40p Pkeet
 * Added the 'd3dcolGetD3DColour' macro.
 * 
 * 26    98/07/20 12:07 Speter
 * Moved some floating point functions to .cpp file.
 * 
 * 25    98/02/11 15:20 Speter
 * Corrected constness of a function.  Added some utility float functions.
 * 
 * 24    6/25/97 7:26p Mlange
 * Now uses the (fast) float to integer conversion functions.
 * 
 * 23    97/06/03 18:36 Speter
 * Much more friendly now for a common include file.  Moved large inline functions to
 * Colour.cpp.  Moved CColourT<> to its own file.  Removed includes.
 * 
 * 22    5/26/97 1:28p Agrant
 * took out automatic conversions between CColourT<type> and CColourT<float> since LINT really
 * didn't like them and they do not
 * appear to be used.
 * 
 * 21    96/10/15 18:19 Speter
 * Changed float parameters to double, to more easily avoid overload ambiguities.
 * 
 * 20    96/10/14 15:32 Speter
 * Changed CColourT<> template to keep components in same range as CColour: 0..255.  This makes
 * for faster conversions in clut building.
 * 
 * 19    96/09/23 16:53 Speter
 * Replaced CFloatColour and CFixedColour with new CColourT template class.
 * 
 * 18    96/08/15 18:43 Speter
 * Added i_dummy parameter to CColour(uint32) to prevent implicit conversions.
 * Changed u4ReverseRGB() to CColour::clrReverseRGB()
 * 
 * 17    8/05/96 6:01p Cwalla
 * Added HSV routines. todo: code layout.
 * 
 * 16    7/22/96 3:30p Mlange
 * 
 * 15    7/19/96 7:03p Mlange
 * Changed iDifference to match colours better, and
 * bias correctly...
 * 
 * 14    96/07/12 17:31 Speter
 * Added u1Intensity() function.
 * 
 * 13    7/09/96 11:07a Pkeet
 * Replaced faulty assembly code in the iDifference function with cpp code.
 * 
 * 12    6/21/96 1:26p Pkeet
 * Added a default constructor for 'CFixedColour.'
 * 
 * 11    6/21/96 10:32a Pkeet
 * Added 'CFloatColour' and 'CFixedColour.'
 * 
 * 10    6/20/96 4:01p Pkeet
 * Rewrote 'iDifference' in assembly.
 * 
 * 9     96/06/12 20:48 Speter
 * Code review changes:
 * Corrected #include guard symbol.
 * Added const to appropriate member functions.
 * Changed CColour::operator+ to simply call Min to clip rather than MinMax.
 * Added lots of comments and to do's.
 * Corrected code alignment.
 * 
 * 8     96/05/31 10:42 Speter
 * Changed colour format back to original BGR order, for compatibility with video surfaces.
 * Added u4ReverseColour function to convert when needed.
 * Added some comments.
 * 
 * 7     96/05/30 17:50 Speter
 * Made Colour + operator do clipping.
 * Changed Colour * operator to take a uint8 instead of an int.
 * Added Asserts to constructors.
 * 
 * 6     5/14/96 7:17p Pkeet
 * Added 'clrBlack' and 'clrWhite' constants.
 * 
 * 5     5/09/96 2:33p Pkeet
 * Changed weights in Difference function to match reds over blues.
 * 
 * 4     5/07/96 1:16p Pkeet
 * Changed colour format to the reflect PALETTEENTRY and not RGBQUAD structure.
 * 
 * 3     5/03/96 4:34p Pkeet
 *  Added '*' (mulitply) given a CColour value. Changed structure to uint8 from TComponent.
 * Changed structure to use Red, Green, Blue from r, g, b. Added the iDifference function.
 * 
 * 2     96/05/03 12:17 Speter
 * Changed clrScale to take a numerator and denominator.
 * Changed operator*(float) to call operator*(int): more efficient.
 * 
 * 1     96/05/01 18:00 Speter
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_VIEW_COLOUR_HPP
#define HEADER_LIB_VIEW_COLOUR_HPP


#if (TARGET_PROCESSOR == PROCESSOR_K6_3D)
	#include "Lib/Math/FloatDef.hpp"
#endif


//**********************************************************************************************
//
class CColour
//
// Represents a 32-bit colour, with 8-bit R, G, and B values.
//
// Format is, from low byte to high: B, G, R, unused.  This is identical to the Intel-standard
// hardware 24-bit pixel format, and to RGBQUAD, which is stored in BMP file palettes.
// It is *not* identical to the PALETTEENTRY structure, which stupidly has R and B reversed.
//
// Can be initialised with either 8-bit integer components, or real components with a range
// of 0 to 1.  Can also be scaled by either 8-bit integers, or reals from 0 to 1.
//
// Prefix: clr
//
//**************************
{
public:
	union
	{
		uint32	  u4Value;		// For accessing the colour as a single 32-bit value.
		struct
		{
			uint8 u1Blue;		// Blue colour component.
			uint8 u1Green;		// Green colour component.
			uint8 u1Red;		// Red colour component.
			uint8 u1Flags;		// Reserved by Windows - should remain unused.
		};
	};

public:

	//******************************************************************************************
	//
	// Constructors and destructor.
	//

	forceinline CColour()
		: u4Value(0)
	{
	}

	// Allow direct copying of a 32-bit value, if the user knows it's a colour.
	forceinline CColour(uint32 u4_val, int i_dummy)
		: u4Value(u4_val)
	{
	}

	// Construct from 8-bit color components.
	forceinline CColour(int i_red, int i_green, int i_blue)
	{
		//
		// Note: arguments are int rather than uint or uint8.  Otherwise, an expression
		// like CColour(100, 200, 80) would generate an ambiguity between the uint and
		// float constructors.
		//
		Assert(bWithin(i_red,   0, 255) &&
			   bWithin(i_green, 0, 255) &&
			   bWithin(i_blue,  0, 255));
		u1Red	= i_red;
		u1Green	= i_green;
		u1Blue	= i_blue;
		u1Flags	= 0;
	}

	// Construct from real components: 1.0 represents maximum intensity (255) of a color component.
	CColour(double d_red, double d_green, double d_blue);

	//******************************************************************************************
	//
	// Assignment operators.
	//

	CColour& operator =(uint32 u4_val)
	{
		u4Value = u4_val;
		return *this;
	}


	//******************************************************************************************
	//
	// Conversion operators.
	//

	operator uint32() const
	{
		return u4Value;
	}


	//******************************************************************************************
	//
	// Operators.
	//

	//
	// Addition of colours.
	//

	CColour operator +(CColour clr) const
	{
		// Add each component, then clip to max value.
		return CColour
		(
			uint8(Min(u1Red   + clr.u1Red,   255)),
			uint8(Min(u1Green + clr.u1Green, 255)),
			uint8(Min(u1Blue  + clr.u1Blue,  255))
		);
	}

	//
	// Multiplication of colours by a scaling value.
	//

	CColour operator *(uint8 u1_scale) const
	{
		//
		// Scale each component by the ratio of u1_scale to 255.
		//
		// Note: this function is only approximately accurate.
		// To be totally accurate, we would have to multiply each component by u1_scale,
		// then divide the product by 255 (the maximum value of u1_scale) rather than 256.
		// However, dividing by 256 is much faster, and should product a colour almost
		// indistinguishable from the theoretically correct colour.
		//
		// When this scaling is replaced with a fast lookup table, it will be exactly accurate.
		//
		return CColour
		(
			//
			// Note: the values are cast to int rather than uint to provide an exact
			// match for the constructor.  Otherwise, a conflict with the float version
			// of the constructor occurs.
			//
			((int)u1Red   * u1_scale) >> 8,
			((int)u1Green * u1_scale) >> 8,
			((int)u1Blue  * u1_scale) >> 8
		);
	}

	CColour operator *(float f_scale) const;

	forceinline CColour ScaleTrunc(float f_scale) const
	{
		Assert(bWithin(f_scale, 0.0f, 1.0f));

	#if (TARGET_PROCESSOR != PROCESSOR_K6_3D)

		float f_red   = float(u1Red);
		float f_green = float(u1Green);
		float f_blue  = float(u1Blue);

		CColour clr;
		uint32 u4_red;
		uint32 u4_green;
		uint32 u4_blue;

		__asm
		{
			// Apply the scale.
			fld		 [f_scale]
			fmul	 [f_red]		// red
			fld		 [f_scale]
			fmul     [f_green]		// green, red
			fxch st(1)				// red, green
			fld		 [f_scale]
			fmul     [f_blue]		// blue, red, green
			fxch st(2)				// green, red, blue

			fistp    [u4_green]		// red, blue
			mov ecx, [u4_green]

			fistp    [u4_red]		// blue
			mov ebx, [u4_red]

			shl ecx, 8

			fistp    [u4_blue]

			shl ebx, 16

			mov eax, [u4_blue]
			or  eax, ecx

			or  eax, ebx

			mov [clr], eax
		}
		return clr;

	#else

		return CColour
		(
			iRound(u1Red   * f_scale),
			iRound(u1Green * f_scale),
			iRound(u1Blue  * f_scale)
		);

	#endif
	}

	CColour operator *(CColour clr_scale) const
	{
		//
		// Scale the colour by an RGB scaling colour.  Each component is scaled by
		// the ratio of the corresponding component of clr_scale to 255.
		//
		// Note: this function is only approximately accurate.
		// See operator *(uint8 u1_scale) above.
		//
		return CColour
		(
			((int)u1Red   * clr_scale.u1Red)   >> 8,
			((int)u1Green * clr_scale.u1Green) >> 8,
			((int)u1Blue  * clr_scale.u1Blue)  >> 8
		);
	}


	//******************************************************************************************
	//
	// Member functions.
	//

	//
	// Access functions.
	//

	//
	// Return equivalent floating-point values, from 0 to 1.
	//

	float fRed() const
	{
		return u1Red * (1.0 / 255.0);
	}

	float fGreen() const
	{
		return u1Green * (1.0 / 255.0);
	}

	float fBlue() const
	{
		return u1Blue * (1.0 / 255.0);
	}

	//******************************************************************************************
	//
	float fIntensity() const;
	//
	// Returns:
	//		The weighted intensity value of the colour, from 0 to 1.
	//
	//**********************************

	//******************************************************************************************
	//
	uint8 u1Intensity() const;
	//
	// Returns:
	//		An integer intensity, from 0 to 255.
	//
	//**********************************

	//******************************************************************************************
	//
	CColour clrScale
	(
		int i_num, int i_denom
	) const
	//
	// Returns:
	//		The colour with each component scaled by the ratio i_num / i_denom.
	//
	//**********************************
	{
		return CColour
		(
			((int)u1Red   * i_num) / i_denom,
			((int)u1Green * i_num) / i_denom,
			((int)u1Blue  * i_num) / i_denom
		);
	}

	//******************************************************************************************
	//
	CColour clrReverseRGB() const
	//
	// Returns:
	//		The colour, with R and B bytes reversed.
	//
	// Notes:
	//		This function is useful in converting CColour to and from Windows PALETTEENTRY
	//		and COLORREF values, which are reversed from the correct order.
	//
	//**********************************
	{
		return CColour(u1Blue, u1Green, u1Red);
	}

	//******************************************************************************************
	//
	uint32 d3dcolGetD3DColour() const
	//
	// Returns this colour in 'D3DCOLOR' form.
	//
	// Notes:
	//		This function is only for abstracting the relationship between 'CColour' and
	//		'D3DCOLOR.' Currently RGBA values are organized in CColour the same way as they are
	//		organized for D3DCOLOR.
	//
	//**********************************
	{
		return u4Value;
	}

};

//
// Global functions.
//

//******************************************************************************************
//
int iDifference
(
	CColour clr_a,	// Colour to match from.
	CColour clr_b	// Colour to match to.
);
//
// Provides a value determining the proximity of one colour against another. The simple
// algorithm provides only a match based on the differences of the two colours.
//
// Charlie: 
// Changed to return a better aprox. for the distance between two colours, in RGB space
// and bias`s  towards G then R then B
//
// Returns.
//		Returns a value representing the differences between the two colours. The larger the
//		returned value, the greater the difference between the two colours.
//
//**********************************

//
// Colour Constants.
//

const CColour clrBlack(0, 0, 0);
const CColour clrWhite(255, 255, 255);

#endif
