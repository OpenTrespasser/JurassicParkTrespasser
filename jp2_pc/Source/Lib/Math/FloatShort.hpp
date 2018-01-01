/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1998.
 *
 * Contents:
 *	 	Contains an object describing a 16 bit value.
 *
 * Bugs:
 *
 * To do:
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Math/FloatShort.hpp                                              $
 * 
 * 2     8/26/98 8:35p Pkeet
 * Added the 'u2Get' member function.
 * 
 * 1     8/26/98 7:55p Pkeet
 * Initial implementation.
 * 
 *********************************************************************************************/

#ifndef HEADER_LIB_MATH_FLOATSHORT_HPP
#define HEADER_LIB_MATH_FLOATSHORT_HPP


//
// Class definitions.
//

//**********************************************************************************************
//
class CShortFloat
//
// A 16 bit floating point number.
//
// Prefix: sf
//
//**************************************
{
private:

	uint16 u2Data;	// 16 bit storage.
	
public:
	
	//******************************************************************************************
	//
	// Overloaded operators.
	//

	//******************************************************************************************
	operator float() const
	{
		uint32 u4 = uint32(u2Data) << 16;
		return fToFloat(u4);
	}

	//******************************************************************************************
	float operator =(float f)
	{
		u2Data = u4FromFloat(f) >> 16;
		return f;
	}

	
	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	uint16 u2Get
	(
	) const
	//
	// Returns the floating point bit pattern as a 16 bit integer.
	//
	//**************************************
	{
		return u2Data;
	}

	//******************************************************************************************
	//
	uint32 u4Get
	(
	) const
	//
	// Returns the floating point bit pattern as a 32 bit integer.
	//
	//**************************************
	{
		uint32 u4 = uint32(u2Data) << 16;
		return u4;
	}

};


#endif // HEADER_LIB_MATH_FLOATSHORT_HPP