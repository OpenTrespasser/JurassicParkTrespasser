/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Definition of the CAngle class with member functions for fast, low precision
 *		trigonometry functions.
 *
 * Bugs:
 *
 * To do:
 *		This file is incomplete! The current implementation is just simple wrapper functions
 *		for the ANSI trig functions. Therefore, the performance of the functions in this
 *		module is slow. The final implementation will be a lot faster, but will have less
 *		precision.
 *
 *		Implement the actual versions of the trig functions that use lookup tables for faster
 *		performance.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Math/FastTrig.hpp                                                 $
 * 
 * 7     6/25/97 5:23p Mlange
 * Now uses iFloatCast.
 * 
 * 6     97/01/16 18:30 Speter
 * Added fTan() function, and dDEGREES constant for conversion.
 * 
 * 5     96/07/22 15:26 Speter
 * Changed * and / operators to take int parameter rather than CAngle.
 * 
 * 4     7/15/96 12:16p Mlange
 * Changes from code review: Added and updated some comments. Changed the name of the
 * initialisation class to CInitTrigTables. Overloaded the multiplication and division operators
 * for the angle class.
 * 
 * 3     3-06-96 5:58p Mlange
 * The CTrigTables class has been updated so that initialisation of the lookup tables will
 * always take place before any global constructors that may depend on it. Changes from code
 * review: Added several constants based on pi. Removed the constructor, assignment and
 * conversion operators for integer angles. Added global functions for conversions between
 * degrees and radians. Removed named member functions for setting and returning the angle in
 * radians and degrees. Removed assert in overloaded negation operator. Made the sine and cosine
 * global functions member functions of the angle class.
 * 
 * 2     16-05-96 3:10p Mlange
 * Updated for changes to angle class.
 * 
 * 1     5/09/96 1:50p Mlange
 * Fast trig functions.
 *
 **********************************************************************************************/

#ifndef HEADER_LIB_MATH_FASTTRIG_HPP
#define HEADER_LIB_MATH_FASTTRIG_HPP

#include "Lib/Math/FloatDef.hpp"
#include <math.h>


//**********************************************************************************************
//
// Constants for class CInitTrigTables.
//

// Size of the combined sine and cosine lookup table.
#define iSIN_TABLE_SIZE_BITS	1
#define iSIN_TABLE_SIZE			(1 << iSIN_TABLE_SIZE_BITS)



//**********************************************************************************************
//
class CInitTrigTables
//
// Class for initialising the trigonometry functions lookup tables.
//
// Prefix: itgt
//
// Notes:
//		This class only contains a constructor that is responsible for initialising the trig
//		tables. A single instance of this class will transparently initialise these tables at
//		startup.
//
//		This header file also contains a declaration for an instance of this class. This is to
//		ensure that the lookup tables will be initialised before any other global constructors
//		that may depend on it.
//
//**************************************
{
private:
	
public:
	//******************************************************************************************
	//
	// Constructors and destructor.
	//

	// The default constructor.
	CInitTrigTables();
};


// The ore module instance of this class.
static CInitTrigTables itgtInstance;


// The lookup tables.
extern float fSinTable[iSIN_TABLE_SIZE];




//**********************************************************************************************
//
// Constants for class CAngle.
//

#define dPI		3.14159265358979324
#define d2_PI	6.28318530717958648
#define dPI_2	1.57079632679489662
#define dPI_3	1.04719755119659775
#define dPI_4	.785398163397448310
#define dPI_6	.523598775598298874

// The number of possible unique angles, expressed as a double.
#define dNUM_ANGLES		(UTypeMax(uint32) + 1.0)



//**********************************************************************************************
//
class CAngle
//
// Type representing Euler angles.
//
// Prefix: ang
//
// Notes:
//		Internally the angle type is represented by a signed integer. The full numerical range
//		of this integer is used to represent the possible angles.
//
//		This means that angle expressed by the CAngle type are always normalised (e.g. it is
//		impossible for angles to be outside of their domain; it is never necessary to perform
//		the CAngle equivalent of: d_radians = fmod(d_radians, d2_PI) ).
//
//		Note that not all the operators are overloaded for this class, most notably, the
//		multiplication and division operators are not defined.
//
//		Because of the limitations of the twos-complement notation, the range of the CAngle
//		type is [-dPI, dPI). Therefore, setting the angle to exactly dPI radians results in the
//		NEGATIVE minimum angle. Angles outside the range of the CAngle type are normalised to
//		lie within this range.
//
//		When initialising this angle with a constant that is outside the range, the compiler
//		may generate warnings for overflows in constant arithmetic.
//
//**************************************
{
private:
	int32 i4Angle;		// The current angle value.

public:
	//******************************************************************************************
	//
	// Constructors and destructor.
	//

	CAngle()
	{
	}

	// Constructor for initialising with an angle specified in radians.
	// Note that this (implicitly) uses the overloaded assignment operator defined below.
	CAngle(double d_radians)
	{
		*this = d_radians;
	}



	//******************************************************************************************
	//
	// Assignment operators.
	//

	CAngle& operator =(double d_radians)
	{
		i4Angle = iFloatCast(float(d_radians / d2_PI * dNUM_ANGLES));

		return *this;
	}



	//******************************************************************************************
	//
	// Conversion operators.
	//
	// Note: both the assignment and conversion operators are defined for the CAngle class.
	// This will cause ambiguities when using mixed types with binary operators. For example:
	// ang_a + d2_PI. This is easily resolved by explicitly specifing the conversion:
	// ang_a + (CAngle)d2_PI for example.
	//

	// Convert this angle to radians expressed as a double.
	operator double() const
	{
		return (double)i4Angle / dNUM_ANGLES * d2_PI;
	}




	//******************************************************************************************
	//
	// Operators.
	//

	//
	// Relational and equality operators.
	// Note that all the other relational operators (!=, >, etc.) are defined in terms of the
	// equality and less than operator by <function.h>.
	//
	bool operator ==(CAngle ang) const
	{
		return i4Angle == ang.i4Angle;
	}


	bool operator <(CAngle ang) const
	{
		return i4Angle < ang.i4Angle;
	}


	// Shift operators.
	CAngle operator <<(int i) const
	{
		Assert(i >= 0 && i < sizeof(i4Angle) * 8 - 1);

		CAngle ang_temp;

		ang_temp.i4Angle = i4Angle << i;
		return ang_temp;
	}


	CAngle operator >>(int i) const
	{
		Assert(i >= 0 && i < sizeof(i4Angle) * 8 - 1);

		CAngle ang_temp;

		ang_temp.i4Angle = i4Angle >> i;
		return ang_temp;
	}


	CAngle& operator <<=(int i)
	{
		Assert(i >= 0 && i < sizeof(i4Angle) * 8 - 1);

		i4Angle <<= i;
		
		return *this;
	}


	CAngle& operator >>=(int i)
	{
		Assert(i >= 0 && i < sizeof(i4Angle) * 8 - 1);

		i4Angle >>= i;

		return *this;
	}


	// Negation operator.
	CAngle operator -() const
	{
		CAngle ang_temp;

		ang_temp.i4Angle = -i4Angle;
		return ang_temp;
	}


	// Additive operators.
	CAngle operator +(CAngle ang) const
	{
		CAngle ang_temp;

		// Note that the (already overloaded) += operator is not used to implement this because it generates inferior code.
		ang_temp.i4Angle = i4Angle + ang.i4Angle;
		return ang_temp;
	}


	CAngle operator -(CAngle ang) const
	{
		CAngle ang_temp;

		// Note that the (already overloaded) -= operator is not used to implement this because it generates inferior code.
		ang_temp.i4Angle = i4Angle - ang.i4Angle;
		return ang_temp;
	}


	// Additive assignment operators.
	CAngle& operator +=(CAngle ang)
	{
		i4Angle += ang.i4Angle;
		
		return *this;
	}

	CAngle& operator -=(CAngle ang)
	{
		i4Angle -= ang.i4Angle;
		
		return *this;
	}


	// Multiplication operators.
	CAngle operator *(int i) const
	{
		CAngle ang_temp;

		ang_temp.i4Angle = i4Angle * i;
		return ang_temp;
	}


	CAngle operator *=(int i)
	{
		i4Angle *= i;

		return *this;
	}


	// Division operators.
	CAngle operator /(int i) const
	{
		CAngle ang_temp;

		ang_temp.i4Angle = i4Angle / i;
		return ang_temp;
	}


	CAngle operator /=(int i)
	{
		i4Angle /= i;

		return *this;
	}


	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	float fSin
	(
	) const
	//
	// Fast, low precision, sine.
	//
	// Returns:
	//		The sine of this angle.
	//
	// Globals:
	//		Requires the sine/cosine table.
	//
	//**************************************
	{
		return (float)sin((double)*this);
	}


	//******************************************************************************************
	//
	float fCos
	(
	) const
	//
	// Fast, low precision, cosine.
	//
	// Returns:
	//		The cosine of this angle.
	//
	// Globals:
	//		Requires the sine/cosine table.
	//
	//**************************************
	{
		return (float)cos((double)*this);
	}

	//******************************************************************************************
	//
	float fTan
	(
	) const
	//
	// Fast, low precision, tangent.
	//
	// Returns:
	//		The tangent of this angle.
	//
	// Globals:
	//		Requires the sine/cosine table.
	//
	//**************************************
	{
		return (float)tan((double)*this);
	}

};


//**********************************************************************************************
//
// Global functions for CAngle.
//

const double dDEGREES = d2_PI / 360.0;

//******************************************************************************************
//
inline double dRadiansToDegrees
(
	double d_radians
)
//
// Convert from radians to degrees.
//
// Returns:
//		The angle expressed in degrees.
//
//**************************************
{
	return d_radians / d2_PI * 360.0;
}


//******************************************************************************************
//
inline double dDegreesToRadians
(
	double d_degrees
)
//
// Convert from degrees to radians.
//
// Returns:
//		The angle expressed in radians.
//
//**************************************
{
	return d_degrees / 360.0 * d2_PI;
}




//******************************************************************************************
//
inline CAngle angArcsin
(
	float f
)
//
// Fast, low precision, arcsine.
//
// Returns:
//		The arcsine of the given parameter.
//
// Globals:
//		Requires the arcsine table.
//
//**************************************
{
	// Ensure the parameter is within the domain.
	Assert(bWithin(f, -1.0f, 1.0f));

	CAngle ang_temp;

	ang_temp = asin((double)f);
	
	return ang_temp;
}


//******************************************************************************************
//
inline CAngle angArccos
(
	float f
)
//
// Fast, low precision, arccosine.
//
// Returns:
//		The arccosine of the given parameter.
//
// Notes:
//		A deviation from the ANSI standard occurs when the parameter is equal to -1.0f. The
//		ANSI standard acos() would return dPI, whereas this function would return the CAngle
//		equivalent of -dPI, since dPI is not representable as a CAngle type.
//
// Globals:
//		Requires the arccosine table.
//
//**************************************
{
	// Ensure the parameter is within the domain.
	Assert(bWithin(f, -1.0f, 1.0f));

	CAngle ang_temp;

	ang_temp = acos((double)f);
	
	return ang_temp;
}



//******************************************************************************************
//
inline CAngle angArctan2
(
	float f_y, float f_x	// The x and y values of the triangle which form the angle for
							// which the Arctan value is to be calculated.
)
//
// Fast, low precision, arctan.
//
// Returns:
//		The angle such that tan(angle) = (y / x).
//
// Notes:
//		If both y and x are zero, this function will return a zero angle, as opposed to the
//		ANSI specified atan2() function, which will produce a domain error in the same
//		situation.
//
// Globals:
//		Requires the arctan table.
//
//**************************************
{
	// Check for undefined lengths.
	if (f_x == 0.0f && f_y == 0.0f)
		return (CAngle)0;

	CAngle ang_temp;

	ang_temp = atan2((double)f_y, (double)f_x);
	
	return ang_temp;
}


//******************************************************************************************
//
inline CAngle angArctan2Length
(
	float f_y, float f_x,	// The x and y values of the triangle which form the angle for
							// which the Arctan value is to be calculated.
	float& rf_length		// Reference to the variable to use to return the length of the
							// hypotenuse.
)
//
// Fast, low precision, arctan and the hypotenuse of the triangle formed.
//
// Returns:
//		The angle such that tan(angle) = (y / x).
//
// Notes:
//		If both y and x are zero, this function will return a zero angle and length.
//
// Side effects:
//		The third parameter is passed by reference and is modified.
//
// Cross references:
//		Requires angArctan2(), fSin(), fCos().
//
//**************************************
{
	// Check for undefined lengths.
	if (f_x == 0.0f && f_y == 0.0f)
	{
		rf_length = 0.0f;
		return (CAngle)0.0;
	}


	// Calculate the angle.
	CAngle ang_arctan;

	ang_arctan = angArctan2(f_y, f_x);

	//
	// Depending on which has the greatest magnitude, calculate the length using the sine or
	// cosine of the angle.
	//
	if (Abs(f_x) > Abs(f_y))
		rf_length = Abs(f_x / ang_arctan.fCos());
	else
		rf_length = Abs(f_y / ang_arctan.fSin());
	
	// Return the arc tangent value.
	return ang_arctan; 		
}


#endif
