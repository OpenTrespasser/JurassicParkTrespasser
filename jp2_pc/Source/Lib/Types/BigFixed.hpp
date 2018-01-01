/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Definition of a 64 bit fixed point type. This fixed point type is a signed quantity
 *		with 32 bits of integer and 32 bits of fractional precision.
 *
 * Bugs:
 *
 * Notes:
 *		While it may be possible to use the int64/double definitions in 'FloatDef.hpp,' this
 *		module will be deleted when a fixed point template becomes available.
 *
 * To do:
 *		Remove this module when a fixed point template becomes available.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Types/BigFixed.hpp                                                $
 * 
 * 5     98.09.19 12:39a Mmouni
 * Added "forceinline" to many small inline functions.
 * Inlined some functions for speed.
 * 
 * 4     12/09/96 1:27p Pkeet
 * Added the dGet member function.
 * 
 * 3     8/12/96 6:34p Pkeet
 * Added to the 'To Do' list.
 * 
 * 2     7/08/96 7:32p Pkeet
 * Changed prefix to 'bfx.'
 * 
 * 1     7/08/96 7:28p Pkeet
 * Initial implementation.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_TYPES_BIGFIXED_HPP
#define HEADER_LIB_TYPES_BIGFIXED_HPP


//
// Constants used to convert from doubles to UBigFixed values.
//

// The mask for mantissa bits in the upper 32-bit word of UBigFixed.
const int i4HighDoubleMantissaMask = 0x000FFFFF;

// The value to cause a double's bits to shift into a 32.32 format.
const double dBigFixedShiftMantissa = (double)(1 << 20);

// Constant for converting fractional portions to double.
const double dFracMultiplier = 1.0 / double((uint64)1 << 32);


//**********************************************************************************************
//
union UBigFixed
//
// Definition of the 32.32 fixed point type and associated overloaded operators and member
// functions.
//
// Prefix: bfx
//
//**************************************
{
	int64  i8;			// 32.32 fixed point value.
	double d;			// Storage represented as a double for use by conversion functions. 
	struct
	{
		uint32 u4Frac;	// Fractional portion of the fixed point value.
		int32  i4Int;	// Integer portion of the fixed point value.
	};


	//******************************************************************************************
	//
	// Conversion operators.
	//
	forceinline UBigFixed& operator=(double d_val)
	{
		//
		// Fast conversion can be performed only on a positive value. If d_val is negative,
		// negate it and then negate the result in i8.
		//
		if (d_val >= 0.0)
		{
			ConvertFromPosDouble(d_val);
		}
		else
		{
			ConvertFromPosDouble(-d_val);
			i8 = -i8;
		}
		return *this;
	}


	//******************************************************************************************
	//
	// Operators.
	//

	// Addition operators.
	forceinline UBigFixed operator+(UBigFixed bfx) const
	{
		bfx += *this;
		return bfx;
	}

	forceinline UBigFixed& operator+=(const UBigFixed& bfx)
	{
		i8 += bfx.i8;
		return *this;
	}

	// Negation operator.
	forceinline UBigFixed operator-() const
	{
		UBigFixed bfx_return_val;
			
		bfx_return_val.i8 = -i8;
		return bfx_return_val;
	}

	// Conversion to double.
	forceinline double dGet() const
	{
		double d_int  = double(i4Int);
		double d_frac = double(u4Frac) * dFracMultiplier;
		return d_int + d_frac;
	}

protected:		

	//******************************************************************************************
	//
	// Conversion function.
	//

	//******************************************************************************************
	//
	forceinline void ConvertFromPosDouble
	(
		double d_val
	)
	//
	// Converts from a double to the 32.32 fixed point format quickly.
	//
	// Notes:
	//		While this fast conversion technique will round the last bit of the fractional value
	//		of the UBigFixed value, the difference between rounding and truncating is so small
	//		it can be discounted.
	//
	//**************************************
	{
		//
		// Shift d_val by a large value so its fraction component is in the bottom 32 bits and
		// its integer component is in the top 32 bits of i8.
		//
		d = d_val + dBigFixedShiftMantissa;

		// Mask out the exponent of the double.
		i4Int &= i4HighDoubleMantissaMask;
	}
};


#endif
