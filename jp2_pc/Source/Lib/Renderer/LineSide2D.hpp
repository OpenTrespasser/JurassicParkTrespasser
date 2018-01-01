/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997.
 *
 * Contents:
 *		2D line functions.
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_RENDERER_LINESIDE2D_HPP
#define HEADER_LIB_RENDERER_LINESIDE2D_HPP


//
// Required includes.
//
#include "Lib/Transform/Vector.hpp"
#include "Lib/Std/Array.hpp"


//
// Line and point relational defines.
//
#define iSIDE_ON   (0)
#define iSIDE_NEG  (1)
#define iSIDE_POS  (2)
#define iSIDE_BOTH (3)


//
// Global functions.
//

//**********************************************************************************************
//
template<class T> int iLineSide
(
	const T& to,	// Vector to test.
	const T& from,	// Vector to test.
	const T& pt		// Point to test.
)
//
// Returns flags indicating which side of the line the point is on.
//
//**********************************
{
	// Take a cross product and return the Z component.
	float r_side_a = (to.tX - from.tX) * (pt.tY - from.tY);
	float r_side_b = (to.tY - from.tY) * (pt.tX - from.tX);

	// Is it positive or on the line?
	if (r_side_a >= r_side_b)
	{
		// Is it on the line?
		if (u4FromFloat(r_side_a) == u4FromFloat(r_side_b))
			return iSIDE_ON;
		return iSIDE_POS;
	}

	// Is must be it negative.
	return iSIDE_NEG;
}

//**********************************************************************************************
//
template<class T> bool bIntersection2D
(
	T&       rv3,					// Point of intersection in 2D.
	const T& p1,					// Start point of the first line.
	const T& p2,					// End point of the first line.
	const T& p3,					// Start point of the second line.
	const T& p4,					// End point of the second line.
	TReal    r_min_slope = 0.01f	// Threshold slope for parallel lines.
)
//
// Finds the point of intersection between two lines parametrically.
//
// Returns 'true' if the lines intersect, or 'false' if the lines are (or almost) parallel.
//
//**********************************
{
	//
	//      (y4-y3)*(x3-x1)-(x4-x3)*(y3-y1)
	// t = ---------------------------------
	//      (y4-y3)*(x2-x1)-(x4-x3)*(y2-y1)
	//
	TReal x4x3 = p4.tX - p3.tX;
	TReal y4y3 = p4.tY - p3.tY;
	TReal x2x1 = p2.tX - p1.tX;
	TReal y2y1 = p2.tY - p1.tY;
	TReal x3x1 = p3.tX - p1.tX;
	TReal y3y1 = p3.tY - p1.tY;

	// Find the denominator of t.
	TReal t_denom = y4y3 * x2x1 - x4x3 * y2y1;

	// If the slope is too small, indicate that lines are (or almost) parallel.
	if (Abs(t_denom) < r_min_slope)
		return false;

	// Finish calculating t.
	TReal t = (y4y3 * x3x1 - x4x3 * y3y1) / t_denom;

	//
	// x = x1 + t * (x2 - x1)
	// y = y1 + t * (y2 - y1)
	//
	rv3.tX = p1.tX + t * x2x1;
	rv3.tY = p1.tY + t * y2y1;
	return true;
}

//**********************************************************************************************
//
template<class T> TReal rTriangleArea2D2
(
	const T& p1,	// Points defining the triangle.
	const T& p2,
	const T& p3
)
//
// Returns the twice area of the triangle defined by the points.
//
// Notes:
//		To find the true area of the triangle, divide the result by two.
//
//**********************************
{
	//
	// Formula:
	//		Area = (x2 - x1) * (x3 - x1) - (y3 - y1) * (y2 - y1)
	//
	// Note the reverse order of the screen vertices. Also note that the CVector2D cross
	// product could be used here.
	//
	return (p2.tX - p1.tX) * (p3.tX - p1.tX) -
		   (p3.tY - p1.tY) * (p2.tY - p1.tY);
}

//**********************************************************************************************
//
template<class T> TReal rLineLengthApprox2D
//
// Returns an approximation of the distance between the two lines.
//
// Notes:
//		Uses Manhattan distance.
//
//**********************************
(
	const T& p1,	// Point to find distance from.
	const T& p2		// Point to find distance to.
)
{
	//
	// Find the manhattan distance.
	//
	float f_dx = Abs(p1.tX - p2.tX);
	float f_dy = Abs(p1.tY - p2.tY);

	if (u4FromFloat(f_dx) > u4FromFloat(f_dy))
		return f_dx + f_dy * 0.4f;
	return f_dy + f_dx * 0.4f;
}

//**********************************************************************************************
//
void GrahamScan
(
	CPArray< CVector2<> >& rpav2,	// Array of vectors to sort.
	TReal r_scale,					// Projection to screen scale factor.
	const CVector2<>& v2_min, 
	const CVector2<>& v2_max
);
//
// Orders the points in counter clockwise direction.
//
//**********************************


#endif // HEADER_LIB_RENDERER_LINESIDE2D_HPP
