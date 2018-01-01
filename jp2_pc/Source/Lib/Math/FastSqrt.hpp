/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Fast, low precision floating point square root and inverse square root functions.
 *
 * Bugs:
 *
 * To do:
 *		Add additional convergence step for increased accuracy.
 *
 *		Consider if a weighted average in the table initialisation would increase the accuracy.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Math/FastSqrt.hpp                                                 $
 * 
 * 12    98/08/25 19:08 Speter
 * Removed unnecessary test for 0.0 in fSqrt.
 * 
 * 11    98/01/06 18:16 Speter
 * Fixed call to fSqrt(0).
 * 
 * 10    97.12.10 2:58p Mmouni
 * Created seperate estimate and full precision versions of fSqrt(), fInvSqrt().
 * 
 * 9     97/06/24 2:51p Pkeet
 * Increased the accuracy of the fast inverse sqrt function.
 * 
 * 8     96/11/18 20:47 Speter
 * Added handy Square inline function.
 * 
 * 7     7/15/96 12:07p Mlange
 * Changes from code review: Added and updated some comments. Changed the name of the table
 * initialisation class to CInitSqrtTables.
 * 
 * 6     96/06/20 6:25p Mlange
 * Updated for changes to the CIntFloat class. 
 * 
 * 5     3-06-96 3:41p Mlange
 * Changes from code review. Now uses 32 bit table entries for increased performance and
 * accuracy.
 * 
 * 4     29-05-96 2:35p Mlange
 * Updated so that initialisation of the lookup tables will always take place before any global
 * constructors that may depend on it.
 * 
 * 3     14-05-96 8:17p Mlange
 * Updated for changes to float def constants.
 * 
 * 2     5/09/96 1:49p Mlange
 * Updated the function comment headers.
 * 
 * 1     5/08/96 12:11p Mlange
 * Fast floating point square and inverse square root.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_MATH_FASTSQRT_HPP
#define HEADER_LIB_MATH_FASTSQRT_HPP


#include "Lib/Math/FloatDef.hpp"


//**********************************************************************************************
//
// Constants for class CInitSqrtTables.
//

// Size (number of entries) of the square root mantissa lookup table. This must be a power of two.
#define iFAST_SQRT_TABLE_SIZE_BITS	(8)
#define iFAST_SQRT_TABLE_SIZE		(1 << iFAST_SQRT_TABLE_SIZE_BITS)


// Size (number of entries) of the inverse square root mantissa lookup table. This must be a power of two.
#define iFAST_INV_SQRT_TABLE_SIZE_BITS	(12)
#define iFAST_INV_SQRT_TABLE_SIZE		(1 << iFAST_INV_SQRT_TABLE_SIZE_BITS)



//**********************************************************************************************
//
class CInitSqrtTables
//
// Class for initialising the square root tables.
//
// Prefix: isqt
//
// Notes:
//		This class only contains a constructor that is responsible for initialising the square
//		root and inverse square root tables. A single instance of this class will transparently
//		initialise these tables at startup.
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
	CInitSqrtTables();
};


// The per module instance of this class.
static CInitSqrtTables isqtInstance;


// The lookup tables.
extern int32 ai4SqrtMantissaTable[iFAST_SQRT_TABLE_SIZE];
extern int32 ai4InvSqrtMantissaTable[iFAST_INV_SQRT_TABLE_SIZE];



//******************************************************************************************
//
inline float fSqrtEst
(
	float f
)
//
// Fast, low precision, floating point square root.
//
// Returns:
//		The square root of the parameter.
//
// Globals:
//		Requires the square root table.
//
//**************************************
{
	CIntFloat if_val;
	int32 i4_table_index;

	// Cannot take the square root of negative numbers.
	Assert(f >= 0.0f);

	// Check for special case of square root of zero.
	if (f == 0.0f)
		return 0.0f;


	//
	// The square root of a floating point number:
	//
	//    e                    e/2    1/2
	//   2  * m, is given by: 2    * m
	//
	// For an integer exponent, when the exponent is even it may be adjusted by shifting right by
	// one. When the exponent is odd, the next smaller even value is considered and the mantissa is
	// doubled. The adjusted mantissa is used to index into a table of computed square roots for the
	// mantissa's domain.
	//

	if_val = f;

	// De-normalise the exponent. Note that the sign bit (this is also the floating point sign bit)
	// now denotes the sign of the exponent.
	if_val.i4Int -= (int32)(CIntFloat)1.0f;

	//
	// Determine the table index for the square root of the mantissa. Take the most significant bits
	// of the mantissa and the least significant bit of the exponent and shift these down for the
	// correct index. Note that for odd exponents, including the least significant bit of the exponent
	// as the most significant bit of the mantissa convienently doubles the mantissa when required
	// (see above). It is important that we have a de-normalised exponent at this point because the
	// value of the exponent's bias is odd, so the least significant bit would be inverted otherwise.
	//
	i4_table_index   = if_val & (i4FLOAT_MANT_MASK | BitMask(iFLOAT_EXP_BIT_START));
	i4_table_index >>= iFLOAT_MANT_BIT_WIDTH - iFAST_SQRT_TABLE_SIZE_BITS + 1;

	// Take the square root of the exponent.
	if_val.i4Int >>= 1;

	// Leave only the exponent and its sign.
	if_val.i4Int &= i4FLOAT_SIGN_BIT_MASK | i4FLOAT_EXP_MASK;

	// Take the square root of the mantissa by table lookup and combine it with the exponent computed
	// above.  The table also contains the exponent bias value, so the table lookup for the mantissa
	// and the normalisation of the exponent are performed by the single statement below.
	if_val.i4Int += ai4SqrtMantissaTable[i4_table_index];

	return if_val;
}



//******************************************************************************************
//
inline float fInvSqrtEst
(
	float f
)
//
// Fast, low precision, floating point inverse square root.
//
// Returns:
//		The recipocal square root of the parameter, that is: 1.0 / sqrt(f).
//
// Globals:
//		Requires the inverse square root table.
//
//**************************************
{
	CIntFloat if_val;
	int32 i4_table_index;

	// Cannot take the inverse square root of negative numbers.
	// Allow inverse square root of zero, as the fSqrt function uses this;
	// it will return a very large number.
	Assert(f >= 0.0f);

	//
	// The inverse square root of a floating point number:
	//
	//    e                    -e/2    -1/2
	//   2  * m, is given by: 2     * m
	//
	// For an integer exponent, when the exponent is even it may be adjusted by negation and shifting
	// right by one. When the exponent is odd, the next smaller even value is considered and the
	// mantissa is doubled. The adjusted mantissa is used to index into a table of computed inverse
	// square roots for the mantissa's domain.
	//

	if_val = f;

	//
	// Determine the table index for the inverse square root of the mantissa. Take the most significant
	// bits of the mantissa and the least significant bit of the exponent and shift these down for the
	// correct index. Note that for odd exponents, including the least significant bit of the exponent
	// as the most significant bit of the mantissa convienently doubles the mantissa when required
	// (see above). Note also, that we do not have de-normalised exponent at this point (unlike in the
	// square root function above). This is correct because the exponent must be negated for inverse
	// square roots, which inverts the least significant bit of the exponent.
	//
	i4_table_index   = if_val & (i4FLOAT_MANT_MASK | BitMask(iFLOAT_EXP_BIT_START));
	i4_table_index >>= iFLOAT_MANT_BIT_WIDTH - iFAST_INV_SQRT_TABLE_SIZE_BITS + 1;


	//
	// Take the inverse square root of the exponent. Calculate:
	//
	//   e - EXP_BIAS + 1                                  3 * EXP_BIAS - 1   e
	// - ---------------- + EXP_BIAS, which simplifies to: ---------------- - -
	//           2                                                   2        2
	//

	// Leave only the exponent.
	if_val.i4Int &= i4FLOAT_EXP_MASK;

	if_val = ((3 * iFLOAT_EXP_BIAS - 1) << (iFLOAT_EXP_BIT_START - 1)) - (if_val >> 1);

	// Truncate remainder of the division by two.
	if_val.i4Int &= i4FLOAT_EXP_MASK;

	// Take the inverse square root of the mantissa by table lookup and combine it with the exponent computed above.
	if_val.i4Int |= ai4InvSqrtMantissaTable[i4_table_index];

	return if_val;
}


//******************************************************************************************
//
inline float fInvSqrt
(
	float f
)
//
// Fast, full precision, floating point inverse square root.
//
// Returns:
//		The recipocal square root of the parameter, that is: 1.0 / sqrt(f).
//
// Globals:
//		Requires the inverse square root table.
//
//**************************************
{
	float x_i = fInvSqrtEst(f);

	// Return after one interation of Newton-Raphson.
	return (0.5f * x_i * (3.0f - f * x_i * x_i));
}


//******************************************************************************************
//
inline float fSqrt
(
	float f
)
//
// Fast, full precision, floating point square root.
//
// Returns:
//		The square root of the parameter.
//
// Globals:
//		Requires the square root table.
//
//**************************************
{
	return (fInvSqrt(f) * f);
}


//******************************************************************************************
//
template<class T> inline T Square
(
	T t								// Input value.
)
//
// Returns:
//		The square of t.
//
// Notes:
//		This macro is convenient for avoiding either recalculation of expressions, or
//		creating temporary variables.  It is placed in FastSqrt.hpp as a bonus.
//
//**************************************
{
	return t * t;
}


#endif

