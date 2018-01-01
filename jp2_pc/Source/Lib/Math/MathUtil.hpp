/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Math utility functions.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Math/MathUtil.hpp                                                 $
 * 
 * 2     98/09/23 0:11 Speter
 * Added fLogInterp.
 * 
 * 1     98/08/09 18:22 Speter
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_MATH_MATHUTIL_HPP
#define HEADER_LIB_MATH_MATHUTIL_HPP

#include "FastSqrt.hpp"

//**********************************************************************************************
//
template<class T> inline int iSolveQuadratic
(
	T at_results[2],					// The results, if any.
	T t_a, T t_b, T t_c					// The terms of the quadratic equation.
)
//
// Returns:
//		The number of valid results: 0 to 2.
//		If 1, both results are the same.
//
//**************************************
{
	//
	// As everyone knows, the solution is:
	//		( -b ± sqrt(b² - 4ac) ) / 2a
	//
	T t_det = t_b*t_b - 4.0*t_a*t_c;
	if (t_det < 0.0)
		// No real solution.
		return 0;

	T t_invdenom = -0.5 / t_a;
	t_det = fSqrt(t_det);
	at_results[0] = (t_b + t_det) * t_invdenom;
	at_results[1] = (t_b - t_det) * t_invdenom;

	// Return # distinct results.
	if (t_det == 0.0)
		return 1;
	else
		return 2;
}

//**********************************************************************************************
//
inline float fLogInterp
(
	float f,
	float f_min, float f_max
)
//
// Returns:
//		The geometric interpolant between f_min and f_max corresponding to f.
//
//**************************************
{
	Assert(f > 0.0f);
	Assert(f_min > 0.0f);
	Assert(f_max > f_min);

	// No one said this was fast.
	return log(f / f_min) / log(f_max / f_min);
}

#endif

