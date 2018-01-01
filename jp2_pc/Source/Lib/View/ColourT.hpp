/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		class CColourT<>;
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/View/ColourT.hpp                                                  $
 * 
 * 1     97/06/03 18:57 Speter
 * Moved CColourT<> from Colour.hpp.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_VIEW_COLOURT_HPP
#define HEADER_LIB_VIEW_COLOURT_HPP

#include "Colour.hpp"

//**********************************************************************************************
//
template<class T> class CColourT
//
// Represents a colour, with R, G, and B values of parameterised type.
// The components are in the same range as CColour; i.e. 0..255, for efficiency of conversion.
//
// Prefix: clr
//
//**************************
{
public:
	T tBlue, tGreen, tRed;				// Colour components, in range 0..255.

public:

	//******************************************************************************************
	//
	// Constructors and destructor.
	//

	CColourT()
		: tBlue(0), tGreen(0), tRed(0)
	{
	}

	// Construct from color components.
	CColourT(T t_red, T t_green, T t_blue)
	{
		// We do not range-check the colours here, because it is useful to have negative
		// or overpositive colours for intermediate calculations.
		tRed	= t_red;
		tGreen	= t_green;
		tBlue	= t_blue;
	}

	// Construct from CColour
	CColourT(CColour clr)
	{
		*this = clr;
	}

	//******************************************************************************************
	//
	// Assignment operators.
	//

	CColourT<T>& operator =(CColour clr)
	{
		tRed   = T(clr.u1Red);
		tGreen = T(clr.u1Green);
		tBlue  = T(clr.u1Blue);
		return *this;
	}

	CColourT<T>& operator =(const CColourT<T>& clr)
	{
		tRed   = clr.tRed;
		tGreen = clr.tGreen;
		tBlue  = clr.tBlue;
		return *this;
	}

	//******************************************************************************************
	//
	// Conversion operators.
	//

	operator CColour() const
	{
		return CColour(int(tRed), int(tGreen), int(tBlue));
	}

	//******************************************************************************************
	//
	// Operators.
	//

	//
	// Addition of colours.
	//

	CColourT<T> operator +(CColourT<T> clr) const
	{
		// Add each component.
		return CColourT<T>
		(
			tRed   + clr.tRed,
			tGreen + clr.tGreen,
			tBlue  + clr.tBlue
		);
	}

	CColourT<T> operator -(CColourT<T> clr) const
	{
		// Subtract each component.
		return CColourT<T>
		(
			tRed   - clr.tRed,
			tGreen - clr.tGreen,
			tBlue  - clr.tBlue
		);
	}

	CColourT<T>& operator +=(CColourT<T> clr)
	{
		// Add each component.
		tRed   += clr.tRed;
		tGreen += clr.tGreen;
		tBlue  += clr.tBlue;
		return *this;
	}

	CColourT<T>& operator -=(CColourT<T> clr)
	{
		// Subtract each component.
		tRed   -= clr.tRed;
		tGreen -= clr.tGreen;
		tBlue  -= clr.tBlue;
		return *this;
	}

	//
	// Multiplication of colours by a scaling value.
	//

	CColourT<T> operator *(T t_scale) const
	{
		//
		// Scale each component by t_scale.
		//
		return CColourT<T>
		(
			tRed   * t_scale, 
			tGreen * t_scale, 
			tBlue  * t_scale
		);
	}

	CColourT<T> operator /(T t_scale) const
	{
		return *this * (T(1) / t_scale);
	}

	CColourT<T> operator *(CColourT<T> clr_scale) const
	{
		//
		// Scale the colour by an RGB scaling colour.  Each component is scaled by
		// the corresponding component of clr_scale.
		//
		return CColourT<T>
		(
			tRed   * clr_scale.tRed    / T(255),
			tGreen * clr_scale.tGreen  / T(255),
			tBlue  * clr_scale.tBlue   / T(255)
		);
	}
};


//**********************************************************************************************
template <class T, class U> inline CColourT<T> clrConvert
(
	T,								// Dummy parameter indicating return type.
	const CColourT<U>& clr			// Colour to convert to different type.
)
//
// Returns:
//		The same colour, in new format.
//
//**************************************
{
	return CColourT<T>(clr.tRed, clr.tGreen, clr.tBlue);
}

#endif
