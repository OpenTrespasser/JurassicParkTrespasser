/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Contents:
 *		CLineSegment
 *
 * Bugs:
 *
 * To do:
 *		Line intersection tests return false positives for long segments with endpoints that
 *			are very close together.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/GeomDBase/LineSegment.hpp                                         $
 * 
 * 8     8/28/97 4:13p Agrant
 * Source Safe Restored to Tuesday, August 26, 1997
 * 
 * 8     97/08/22 6:45p Pkeet
 * Removed a template parameter.
 * 
 * 7     5/26/97 1:39p Agrant
 * LINT fix.
 * 
 * 6     97/05/21 18:00 Speter
 * Updated for new CFuzzy<> template param.
 * 
 * 5     97/04/25 12:24p Pkeet
 * Removed the 'bEquals' member function.
 * 
 * 4     4/21/97 11:56a Agrant
 * Comments about an intersection bug
 * 
 * 3     97/04/08 2:40p Pkeet
 * Added the 'bEquals' member function.
 * 
 * 2     2/16/97 5:36p Agrant
 * Segment intersections now handle points on the line better.
 * 
 * 1     2/10/97 11:30a Agrant
 * initial rev
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_GEOMDBASE_LINESEGMENT_HPP
#define HEADER_LIB_GEOMDBASE_LINESEGMENT_HPP

#include "Lib/Std/StdLibEx.hpp"

//**********************************************************************************************
//
template <class TREAL = TReal>  //lint !e1048
class CLineSegment2
//
// Prefix: seg
//
//	A class that describes a 2d line segment.
//
//**************************************
{
public:
	//
	//	Variables.
	//

	CVector2<TREAL> v2From;		// Endpoints of the segment.
	CVector2<TREAL> v2To;


	//******************************************************************************************
	//
	// Constructors and destructor.
	//

	CLineSegment2(const CVector2<TREAL>& v2_1, const CVector2<TREAL>& v2_2) :
		v2From(v2_1), v2To(v2_2)
	{
	}

	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	TREAL tSide
	(
		const CVector2<TREAL> v2_point
	) const
	//
	// Returns a value describing the position of the point relative to the line defined by the segment.
	//
	//	Returns:
	//		Value is zero if the point lies on the line.
	//		Value is positive if the point lies to the left of the line, when forward is defined
	//			by looking from "v2From" to "v2To".
	//		Value is negative if the point lies to the right of the line, viewed as above.
	//
	//**********************************
	{
		CVector2<TREAL> v2_line_direction	= v2To		- v2From;
		CVector2<TREAL> v2_point_direction	= v2_point	- v2From;

		// Take a cross product and return the Z component.
		return v2_line_direction.tX * v2_point_direction.tY - 
			   v2_line_direction.tY * v2_point_direction.tX; 
	}


	//******************************************************************************************
	//
	bool bIntersects
	(
		const CVector2<TREAL>& v2_point
	) const
	//
	//	Returns true if the point intersects the line segment, but see Notes.
	//
	//	Notes:
	//		Assumes that the point lies on the line defined by the line segment.
	//
	//**********************************
	{
		if (Fuzzy(v2From.tX) == v2To.tX)
		{
			// Then use Y to test.
			if (v2_point.tY < v2From.tY && v2_point.tY < v2To.tY)
				return false;
			else if (v2_point.tY > v2From.tY && v2_point.tY > v2To.tY)
				return false;
			else return true;
		}
		else
		{
			// Then use X to test.
			if (v2_point.tX < v2From.tX && v2_point.tX < v2To.tX)
				return false;
			else if (v2_point.tX > v2From.tX && v2_point.tX > v2To.tX)
				return false;
			else return true;
		}
	}


	//******************************************************************************************
	//
	bool bIntersects
	(
		const CLineSegment2<TREAL>& seg
	) const
	//
	// Returns true if the line segments intersect or touch.
	//
	//	Notes:  This function fails on long line segments that have endpoints close
	//		together because of the cross product calculation.  Segments of this type
	//		make for very small cross products involving the close proximity points.
	//		The very small cross products miss the fuzzy zero compare.
	//
	//		Why, then, the fuzzy compare?  The fuzzy compare is necessary to detect
	//		segments which share endpoints, thus allowing the algorithm to return
	//		"true" (there is an intersection) when it encounters segments sharing 
	//		endpoints.
	//
	//		This can be improved dramatically, by making the tolerance smaller, switching
	//		to double precision, or just checking for shared vertices beforehand.
	//
	//		In the meantime, as this code is only used in pathfinding, we can tolerate the 
	//		occasional false positive intersection for long line segments.
	//
	//**********************************
	{
		// Quick and dirty, can be faster.

		// If both points of one line segment are on the same side of the line defined by the other,
		// then they do not intersect.
		
		TREAL t_side_seg_from	= tSide(seg.v2From);
		TREAL t_side_seg_to		= tSide(seg.v2To);

		CFuzzy<TREAL, TREAL> fuzzy_zero(TREAL(0), fTOLERANCE_DEFAULT);


		if (!(fuzzy_zero >= t_side_seg_from * t_side_seg_to))
		{
			// Both points are to the right
			// or both points are to the left
			return false;
		}

		// Then check to see how "this" lies relative to "seg"
		// If we have opposite signs, return true
		// If we have zero, return true
		TREAL t_side_this_from	= seg.tSide(v2From);
		TREAL t_side_this_to	= seg.tSide(v2To);
		
		if (!(fuzzy_zero >= t_side_this_from * t_side_this_to))
		{
			// Both points are to the right
			// or both points are to the left
			return false;	
		}

		// The remaining cases are colinear segments, intersecting segments, and segments where
		// an endpoint of one seg lies on the other segment.
		if (fuzzy_zero == t_side_seg_from)
		{
			// Then from point is on this line, but maybe not this segment!
			if (bIntersects(seg.v2From))
				return true;
		}

		/*
		if (fuzzy_zero == t_side_seg_to)
		{
			// Then from point is on this line, but maybe not this segment!
			if (bIntersects(seg.v2To))
				return true;
		}

		if (fuzzy_zero == t_side_this_from)
		{
			// Then from point is on this line, but maybe not this segment!
			if (seg.bIntersects(v2From))
				return true;
		}

		if (fuzzy_zero == t_side_this_to)
		{
			// Then from point is on this line, but maybe not this segment!
			if (seg.bIntersects(v2To))
				return true;
		}
*/
		return true;		
		
		/*
		// If the segments are co-linear, then we have to do a fancy compare to resolve that case.
		if (fuzzy_zero == t_side_seg_from && fuzzy_zero == t_side_seg_to)
		{
			// Colinear segments!
			
			// Actually, this could be a degenerate case, where one segment is zero length.
			// We'll claim no intersection in that case.
			if ((seg.v2From - seg.v2To).bIsZero() ||
				(v2From		- v2To).bIsZero())
				return false;



			// Can reduce to one dimension, thank you very much.
			TREAL r_seg_max;
			TREAL r_seg_min;
			TREAL r_me_max;
			TREAL r_me_min;

			
#define SORT_ASCENDING(val1, val2, low, high) \
	if (val1 < val2) { low = val1; high = val2; } else { high = val1; low = val2; }

			SORT_ASCENDING(seg.v2From.tX, seg.v2To.tX, r_seg_min, r_seg_max);
			SORT_ASCENDING(v2From.tX, v2To.tX, r_me_min, r_me_max);

			return !(	r_seg_min > r_me_max ||
						r_seg_max < r_me_min);
		}
*/
	}
};

template <class GOOF>
class GOOFY_BUG_FIX {};

//#ifndef HEADER_LIB_GEOMDBASE_LINESEGMENT_HPP
#endif
