/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997.
 *
 * Contents:
 *		Functions to extract ranges from CVector types.
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Transform/VectorRange.hpp                                         $
 * 
 * 5     97/08/13 11:58p Pkeet
 * Fixed bug in 'uGetLargestYIndex.'
 * 
 * 4     97/07/17 3:56p Pkeet
 * Added the 'uGetLargestYIndex' function.
 * 
 * 3     97/07/17 10:22a Pkeet
 * Fixed bug in 'RemoveDuplicatePoints' test.
 * 
 * 2     97/07/16 7:56p Pkeet
 * Added the 'RemoveDuplicatePoints' function.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_TRANSFORM_VECTORRANGE_HPP
#define HEADER_LIB_TRANSFORM_VECTORRANGE_HPP


//
// Required includes.
//
#include "Float.h"
#include "Vector.hpp"
#include "Lib/Std/Array.hpp"


//**********************************************************************************************
//
template<class T> void GetMinMax
(
	const CPArray< T >& pav,		// Array of points to the extents of.
	T&                  rt_min,		// Minimum extent.
	T&                  rt_max		// Maximum extent.
)
//
// Returns the minimum and maximum extents of the array.
//
//**********************************
{
	Assert(pav.atArray);
	Assert(pav.uLen > 0);

	// Iterate through the shape's points setting the min and max values.
	rt_min = rt_max = pav[0];

	for (uint u = 1; u < pav.uLen; u++)
	{
		// Set the min and max values.
		rt_min.SetMin(pav[u]);
		rt_max.SetMax(pav[u]);
	}
}

//**********************************************************************************************
//
template<class TArray, class TExt> void GetMinMax2D
(
	CPArray< TArray > pav,		// Array of points to the extents of.
	TExt&             rt_min,	// Minimum extent.
	TExt&             rt_max	// Maximum extent.
)
//
// Returns the minimum and maximum extents of the array.
//
//**********************************
{
	Assert(pav.atArray);
	Assert(pav.uLen > 0);

	// Iterate through the shape's points setting the min and max values.
	rt_min.tX = rt_max.tX = pav[0].tX;
	rt_min.tY = rt_max.tY = pav[0].tY;

	for (uint u = 1; u < pav.uLen; u++)
	{
		// Set the min and max values.
		rt_min.tX = Min(rt_min.tX, pav[u].tX);
		rt_min.tY = Min(rt_min.tY, pav[u].tY);
		rt_max.tX = Max(rt_max.tX, pav[u].tX);
		rt_max.tY = Max(rt_max.tY, pav[u].tY);
	}
}

//**********************************************************************************************
//
inline void GetMinMaxXZ
(
	const CPArray< CVector3<> >& pav3,		// Array of points to the extents of.
	CVector3<>&                  rv3_min,	// Minimum extent.
	CVector3<>&                  rv3_max	// Maximum extent.
)
//
// Returns the minimum and maximum extents of the array.
//
//**********************************
{
	Assert(pav3.atArray);
	Assert(pav3.uLen > 0);

	// Iterate through the shape's points setting the min and max values.
	rv3_min.tX = rv3_max.tX = pav3[0].tX;
	rv3_min.tZ = rv3_max.tZ = pav3[0].tZ;

	for (uint u = 1; u < pav3.uLen; u++)
	{
		// Set the min and max values.
		rv3_min.tX = Min(rv3_min.tX, pav3[u].tX);
		rv3_min.tZ = Min(rv3_min.tZ, pav3[u].tZ);
		rv3_max.tX = Max(rv3_max.tX, pav3[u].tX);
		rv3_max.tZ = Max(rv3_max.tZ, pav3[u].tZ);
	}
}

//**********************************************************************************************
//
inline void GetMinMaxZ
(
	const CPArray< CVector3<> >& pav3,		// Array of points to the extents of.
	TReal&                       r_zmin,	// Minimum z extent.
	TReal&                       r_zmax		// Maximum z extent.
)
//
// Returns the minimum and maximum z extents of the array.
//
//**********************************
{
	Assert(pav3.atArray);
	Assert(pav3.uLen > 0);

	// Iterate through the shape's points setting the min and max values.
	r_zmin = r_zmax = pav3[0].tZ;

	for (uint u = 1; u < pav3.uLen; u++)
	{
		TReal r_z = pav3[u].tZ;

		// Set the min and max z values.
		if (r_z < r_zmin)
			r_zmin = r_z;
		else
			if (r_z > r_zmax)
				r_zmax = r_z;
	}
}

//**********************************************************************************************
//
inline bool bInsideFloatRange
(
	const CVector3<>& v3	// Coordinates to check extents of.
)
//
// Returns 'true' if the value is not the smallest (negative) or largest (positive) floating
// point number.
//
//**********************************
{
	if (v3.tX == FLT_MAX || v3.tX == -FLT_MAX ||
		v3.tY == FLT_MAX || v3.tY == -FLT_MAX ||
		v3.tZ == FLT_MAX || v3.tZ == -FLT_MAX)
		return false;
	return true;
}

//**********************************************************************************************
//
template<class T> void RemoveDuplicatePoints
(
	CPArray< T >& rpav,					// Array of points to remove duplicates from.
	TReal r_tolerance = TReal(0.001)	// Tolerance for proximity test.
)
//
// Removes duplicate points in the point set.
//
//**********************************
{
	int i = 0;
	for (;;)
	{
		// Do nothing if there is only one point in the set.
		if (rpav.uLen <= 1)
			return;

		// Is the next point close to any of the remaining points.
		bool b_close = false;
		for (int i_rem = i + 1; i_rem < rpav.uLen; i_rem++)
		{
			if (Fuzzy(rpav[i], r_tolerance) == rpav[i_rem])
			{
				b_close = true;
				break;
			}
		}

		// If the point is close, replace it with the end point.
		if (b_close)
		{
			rpav[i] = rpav[rpav.uLen - 1];
			--rpav.uLen;
		}
		else
		{
			// Otherwise increment the current point.
			++i;
		}
		if (i >= int(rpav.uLen) - 2)
			return;
	}
}

//**********************************************************************************************
//
template<class T> uint uGetLargestYIndex
(
	const T& pav	// Array of vectors to test.
)
//
// Returns the highest index value in the array.
//
//**************************************
{
	Assert(pav.atArray);
	Assert(pav.uLen);

	TReal r_y_largest = pav[0].tY;	// Largest Y value.
	uint  u_largest   = 0;			// Index of largest value.

	for (uint u = 1; u < pav.uLen; ++u)
		if (pav[u].tY > r_y_largest)
		{
			// Set for new high values.
			r_y_largest = pav[u].tY;
			u_largest   = u;
		}
	return u_largest;
}


#endif // HEADER_LIB_TRANSFORM_VECTORRANGE_HPP
