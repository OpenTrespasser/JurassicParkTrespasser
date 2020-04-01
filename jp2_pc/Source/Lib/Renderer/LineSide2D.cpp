/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1997.
 *
 * Contents:
 *		Implementation of LineSide2D.hpp.
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/LineSide2D.cpp                                           $
 * 
 * 28    9/24/98 6:01p Asouth
 * moved _alloca out of parameter list
 * 
 * 27    8/27/98 1:50p Asouth
 * Loop variable moved into block scope
 * 
 * 26    5/21/98 10:40p Pkeet
 * Fixed crash bugs.
 * 
 * 25    3/10/98 1:20p Pkeet
 * Added include to "LocalArray.hpp."
 * 
 * 24    2/23/98 2:28p Agrant
 * Made the 'GrahamSubScan' function safer.
 * 
 * 23    97/10/31 5:39p Pkeet
 * Replace floating point comparisons with integer comparisons. Changed threshold values
 * slightly.
 * 
 * 22    97/10/31 5:05p Pkeet
 * Added a test to remove points that are close together and almost colinear.
 * 
 * 21    97/10/31 4:08p Pkeet
 * Made the dot product test work.
 * 
 * 20    97/10/30 7:07p Pkeet
 * Fixed compile bug.
 * 
 * 19    97/10/30 7:03p Pkeet
 * Added the 'RemoveSameAnglePoints' function.
 * 
 * 18    97/10/30 11:14a Pkeet
 * Changed the code that removes close point to scale according to the minimum dimensions of the
 * point set.
 * 
 * 17    97/10/29 4:00p Pkeet
 * Implemented the 'RemoveClosePoints' function.
 * 
 * 16    8/19/97 7:02p Bbell
 * Uncommented simplification algorithm.
 * 
 * 15    8/19/97 11:28a Bbell
 * Disabled simplification algorithm.
 * 
 * 14    8/18/97 11:55p Pkeet
 * Fixed the simplification routine.
 * 
 * 13    8/18/97 11:44a Pkeet
 * Removed profile stats.
 * 
 * 12    8/16/97 2:33p Pkeet
 * Commented out the simplify call temporarily to prevent a crash bug.
 * 
 * 11    97/08/13 10:56p Pkeet
 * Fixed a bug with the initial sort.
 * 
 * 10    97/07/28 2:14p Pkeet
 * Still more bugs.
 * 
 * 9     97/07/25 17:00 Speter
 * Moved all definitions of cache profile stats to RenderCache.cpp, to solve initialisation
 * dependency problems.
 * 
 * 8     97/07/23 11:20a Pkeet
 * Fixed bug in Graham's scan.
 * 
 * 7     97/07/18 11:56a Pkeet
 * Limited the size of the border.
 * 
 * 6     97/07/17 4:48p Pkeet
 * Cleaned up and commented code. Replaced the map type used for sorting in RadialSort with a
 * sort array. Interleaved slope and segment information for the radial sort.
 * 
 **********************************************************************************************/


//
// Required includes.
//
#include <math.h>
#include <algorithm>
#include <set>
#include "Common.hpp"
#include "Lib/Std/CircularList.hpp"
#include "LineSide2D.hpp"
#include "Lib/Sys/textout.hpp"
#include "Lib/Sys/Profile.hpp"
#include "Lib/Renderer/RenderCacheInterface.hpp"
#include "LineSide2DTest.hpp"
#include "Lib/Transform/VectorRange.hpp"
#include "Lib/Std/LocalArray.hpp"


//
// Type definitions.
//

// An STL-style container that wraps.
typedef CCircularList< CVector2<> > TCList;


//
// Constants.
//

// Angle threshold for colinear point elimination.
const float fColinearThresholdSqr = Sqr(0.99975f);

// Distance threshold for colinear point elimination using proximity.
const float fNearThresholdSqr = Sqr(0.05f);

// Angle threshold for colinear point elimination.
const float fNearThresholdAngleSqr = Sqr(0.995f);


//
// Function prototypes.
//

//**********************************************************************************************
//
void GrahamSubScan
(
	TCList& clist	// List to remove points from.
);
//
// Removes points from the list that do not contribute to a convex polygon.
//
//**********************************

//**********************************************************************************************
//
void Simplify
(
	TCList& clist,		// List of convex points to simplify.
	TReal r_scale,		// Point to screen point conversion scale.
	CVector2<> v2_min,	// Minimum bounding rectangle.
	CVector2<> v2_max	// Maximum bounding rectangle.
);
//
// Simplifies a convex polygon by sides that do not reduce the overall area by much.
//
//**********************************

//**********************************************************************************************
//
void RadialOrder
(
	CPArray< CVector2<> >& rpav2,	// Input array of points to sort.
	TCList& clist					// Output list of points.
);
//
// Orders the points in counter clockwise direction from the topmost point.
//
//**********************************


//
// Class definitions.
//

//**********************************************************************************************
//
class CRadial
//
// Object for sorting points radially around a point.
//
// Prefix: rad
//
// Notes:
//		Instead of doing a conversion to polar coordinates (an angle), the angle is represented
//		by a segment value and a slope (dx/dy or dy/dx).
// 
//   Points are first place into a segment:
// 
// 			------------------
// 			|\       |      / |
// 			| \   7  |  0  /  |
// 			|  \     |    /   |
// 			|   \    |   /    |
// 			| 6  \   |  /   1 |
// 			|     \  | /      |
// 			|      \ |/       | 
// 			|-----------------
// 			|       /|\       |
// 			|      / | \      |
// 			| 5   /  |  \   2 |
// 			|    /   |   \    |
// 			|   /    |    \   |
// 			|  /     |     \  |
// 			| /  4   |   3  \ |
// 			|/       |       \|
// 			-------------------
//
//**********************************
{
public:

	uint32      u4BinAngle;	// Combined segment and slope data.
	CVector2<>* pv2Point;	// Pointer to the original vector.

public:

	//******************************************************************************************
	//
	// Constructors.
	//

	// Default constructor.
	CRadial()
	{
	}

	// Constructor from a point and a centre point.
	CRadial(const CVector2<>& v2_centre, CVector2<>* pv2_pt)
	{
		uint32 u4_slope;	// Gradient information.
		pv2Point = pv2_pt;

		float f_x = pv2_pt->tX - v2_centre.tX;
		float f_y = pv2_pt->tY - v2_centre.tY;

		if (f_x == 0.0f && f_y == 0.0f)
		{
			u4BinAngle = 0;
			return;
		}

		//
		// Select the segment the point lies within.
		//

		// Half the number of possible segements.
		if (f_x >= TReal(0))
		{
			// Choose from segments 0 to 3.
			if (f_y >= TReal(0))
			{
				// Choose from segments 0 and 1.
				if (f_y > f_x)
					u4BinAngle = 0;
				else
					u4BinAngle = 1;
			}
			else
			{
				// Choose from segments 2 and 3.
				if (f_y > -f_x)
					u4BinAngle = 2;
				else
					u4BinAngle = 3;
			}
		}
		else
		{
			// Choose from segments 4 to 7.
			if (f_y >= TReal(0))
			{
				// Choose from segments 6 and 7.
				if (f_y > -f_x)
					u4BinAngle = 7;
				else
					u4BinAngle = 6;
			}
			else
			{
				// Choose from segments 4 and 5.
				if (-f_y > -f_x)
					u4BinAngle = 4;
				else
					u4BinAngle = 5;
			}
		}

		//
		// Calculate the slope appropriate to the segment.
		//
		float f_slope;
		switch (u4BinAngle)
		{
			case 0:
			case 3:
			case 4:
			case 7:
				f_slope = f_x / f_y;
				break;
			case 1:
			case 2:
			case 5:
			case 6:
				f_slope = f_y / f_x;
				break;
			default:
				Assert(0);
		}
		f_slope = Abs(f_slope);
		u4_slope = u4FromFloat(f_slope);

		// Combine the segement with the slope for a single value that can be compared.
		if (u4BinAngle & 1)
			u4_slope = ~u4_slope;
		u4_slope &= 0x7FFFFFFF;
		u4_slope >>= 14;
		u4BinAngle = (u4BinAngle << 28) | u4_slope;
	}

	//******************************************************************************************
	//
	// Member functions.
	//

	//**************************************************************************************
	//
	bool operator()(const CRadial& rad_0, const CRadial& rad_1) const
	//
	// Returns 'true' angle '0' is clockwise from angle '1.'
	//
	//**************************************
	{
		return rad_0.u4BinAngle > rad_1.u4BinAngle;
	}

};


//
// Function implementations.
//

//**********************************************************************************************
float fGetScale(const TCList& clist)
{
	Assert(clist.size() > 2);

	// Initialize with the first point in the list.
	TCList::iterator it = clist.begin();

	CVector2<> v2_min = *it;
	CVector2<> v2_max = *it;

	// Increment to the next point.
	++it;

	// Compare all of the points in the circular buffer.
	for (; it != clist.end(); ++it)
	{
		v2_min.tX = Min(v2_min.tX, (*it).tX);
		v2_min.tY = Min(v2_min.tY, (*it).tY);
		v2_max.tX = Max(v2_max.tX, (*it).tX);
		v2_max.tY = Max(v2_max.tY, (*it).tY);
	}

	// Get the smallest delta.
	return Min(v2_max.tX - v2_min.tX, v2_max.tY - v2_min.tY);
}

//**********************************************************************************************
//
inline bool bColinear
(
	const CVector2<>& v2_a,	// End point.
	const CVector2<>& v2_b,	// Middle point.
	const CVector2<>& v2_c,	// End point.
	float f_threshold
)
//
// Returns 'true' if the middle point is on the line formed by the two end points.
//
// Notes:
//		Uses the dot product. To avoid normalizing the two vectors, the threshold value
//		is multiplied by the squares of the two lines.
//
//**************************************
{
	bool b_retval = false;

	CVector2<> v2_line_a = v2_a - v2_b;
	CVector2<> v2_line_b = v2_c - v2_b;

	float f_dot = v2_line_a * v2_line_b;
	
	if (u4FromFloat(f_dot) & 0x80000000)
	{
		float f_dot_sqr = f_dot * f_dot;
		float f_a_sqr   = v2_line_a.tLenSqr();
		float f_b_sqr   = v2_line_b.tLenSqr();
		float f_ab_sqr  = f_a_sqr * f_b_sqr;
		float f_abthresh_sqr = fColinearThresholdSqr * f_ab_sqr;

		b_retval = u4FromFloat(f_dot_sqr) > u4FromFloat(f_abthresh_sqr);

		if (!b_retval)
		{
			if ((u4FromFloat(f_a_sqr) < u4FromFloat(f_threshold)) ||
		        (u4FromFloat(f_b_sqr) < u4FromFloat(f_threshold)))
			{
				f_abthresh_sqr = fNearThresholdAngleSqr * f_ab_sqr;
				b_retval = u4FromFloat(f_dot_sqr) > u4FromFloat(f_abthresh_sqr);
			}
		}
	}

	return b_retval;
}

//**********************************************************************************************
//
void RemoveColinearPoints
(
	TCList& clist,	// Circular buffer to remove points from.
	float   f_scale	// Scale.
)
//
// Returns 'true' if the mid point is colinear with the two other points, or forms a concave
// line.
//
//**************************************
{
	Assert(clist.size() > 2);

	float f_threshold = fNearThresholdSqr * f_scale * f_scale;
	int i_count_since_last_removal = 0;
	TCList::iterator it = clist.begin();

	//
	// Iterate through points until the loop goes once through all the points without
	// eliminating any.
	//
	for (;i_count_since_last_removal <= clist.size();)
	{
		// Get three points.
		TCList::iterator it_next = it;
		++it_next;
		TCList::iterator it_next_next = it_next;
		++it_next_next;

		// If the middle point is colinear, eliminated it.
		if (bColinear(*it, *it_next, *it_next_next, f_threshold))
		{
			// Reset the counter.
			i_count_since_last_removal = 0;

			// Remove the point.
			clist.erase(it_next);
		}
		else
		{
			// Get the next point.
			++i_count_since_last_removal;
			++it;
		}
	}
}

//**********************************************************************************************
void GrahamScan(CPArray< CVector2<> >& rpav2, TReal r_scale, const CVector2<>& v2_min,
				const CVector2<>& v2_max)
{
	Assert(rpav2.atArray);
	Assert(rpav2.uLen > 2);

	// If there are not points to sort, do nothing.
	if (rpav2.uLen < 3)
		return;

	CCycleTimer	ctmr;				// Timer object.
	
	// Create the circular buffer.
	void* clistBuffer = _alloca(rpav2.uLen * 2 * CCircularList<CVector2<>*>::uSizeOfElement());
	TCList clist
	(
		clistBuffer,
		rpav2.uLen * 2
	);

	//
	// Sort points in radial order counter-clockwise from the topmost point.
	//
	RadialOrder(rpav2, clist);
	Assert(clist.size() > 2);

	// Debug and statistics information.
	CVector2<> v2_centre = *clist.begin();
	VerifyRadialOrder(v2_centre, rpav2, clist, (const char*)"Scan1.txt");

	//
	// Remove points that do not contribute to a convex polygon.
	//
	GrahamSubScan(clist);
	Assert(clist.size() > 2);
	VerifyRadialOrder(v2_centre, rpav2, clist, (const char*)"Scan2.txt");

	// Find a range scale.
	float f_scale = fGetScale(clist);

	//
	// Remove points that fall approximately on line.
	//
	RemoveColinearPoints(clist, f_scale);
	Assert(clist.size() > 2);
	VerifyRadialOrder(v2_centre, rpav2, clist, (const char*)"Scan3.txt");

	//
	// Remove points that are not significant in the convex polygon.
	//
	Simplify(clist, r_scale, v2_min, v2_max);
	Assert(clist.size() > 2);
	VerifyRadialOrder(v2_centre, rpav2, clist, (const char*)"Scan4.txt");

	// Rebuild the array.
	uint u = 0;
	TCList::iterator itc = clist.begin();
	do
	{
		rpav2[u++] = *itc;
		++itc;
	}
	while (itc != clist.begin());
	rpav2.uLen = u;
	Assert(rpav2.uLen > 2);

	// Debug and statistics information.
	VerifyRadialOrder(v2_centre, rpav2, clist, (const char*)"ScanFinish.txt");
}

//**********************************************************************************************
void RemovePoint(CPArray<CRadial>& arad, uint u)
{
	Assert(arad.uLen > 1);

	for (; u < arad.uLen - 1; ++u)
		arad[u] = arad[u + 1];
	--arad.uLen;
}

//**********************************************************************************************
void RemoveSameAnglePoints(CPArray<CRadial>& arad, const CVector2<>& v2)
{
	for (;;)
	{
		bool b_removed = false;

		for (uint u = 0; u < arad.uLen - 1;)
		{
			if (arad[u].u4BinAngle == arad[u + 1].u4BinAngle)
			{
				if ((*arad[u].pv2Point - v2).tLenSqr() < (*arad[u + 1].pv2Point - v2).tLenSqr())
					RemovePoint(arad, u);
				else
					RemovePoint(arad, u + 1);
				b_removed = true;
			}
			else
			{
				++u;
			}
		}
		if (!b_removed)
			return;
	}
}

//**********************************************************************************************
void RadialOrder(CPArray< CVector2<> >& rpav2, TCList& clist)
{
	// If there are not points to sort, do nothing.
	if (rpav2.uLen < 2)
		return;

#if bOUTPUT_LINESIDE2D_DATA
	// Open a file to write to.
	CConsoleBuffer con(125, 80);
	con.OpenFileSession("SortPointsArray.txt");
#endif // bOUTPUT_LINESIDE2D_DATA

	CLArray(CRadial, parad, rpav2.uLen);	// Storage for the radial sort array.

	// Find the highest point.
	uint u_highest = uGetLargestYIndex(rpav2);
	CVector2<> v2  = rpav2[u_highest];

	// Add points to the array.
	uint u;
	for (u = 0; u < rpav2.uLen; ++u)
		parad[u] = CRadial(v2, &rpav2[u]);
	
#if bOUTPUT_LINESIDE2D_DATA
	con.Print("\nBefore:\n\n");
	for (int i = 0; i < parad.uLen; ++i)
		PrintPos(v2, *(parad[i].pv2Point), con);
#endif // bOUTPUT_LINESIDE2D_DATA

	// Sort the radial array using STL's QSort routine.
	std::sort(parad.atArray, parad.atArray + parad.uLen, CRadial());

	RemoveSameAnglePoints(parad, v2);

#if bOUTPUT_LINESIDE2D_DATA
	con.Print("\n\n\nAfter:\n\n");
	for (i = 0; i < parad.uLen; ++i)
		PrintPos(v2, *(parad[i].pv2Point), con);
	con.CloseFileSession();
#endif // bOUTPUT_LINESIDE2D_DATA

	for (u = 0; u < parad.uLen; ++u)
		clist.push_back(*parad[u].pv2Point);

	// Find the highest point.
	TCList::iterator it = clist.begin();
	TCList::iterator it_highest = it;
	++it;
	for (; it != clist.end(); ++it)
	{
		if ((*it).tY > (*it_highest).tY)
			it_highest = it;
	}

	// Set the beginning of the list to the highest point.
	clist.SetBegin(it_highest);
}

//**********************************************************************************************
void GrahamSubScan(TCList& clist)
{

	TCList::iterator it_a = clist.begin();
	for (;;)
	{
		// If the list has three points or less, there is no point to a scan.
		if (clist.size() <= 4)
			return;

		// Get the middle point.
		TCList::iterator it_b = it_a.itNext();

		// Get the end point.
		TCList::iterator it_c = it_b.itNext();

		// Try to remove point b.
		if (iLineSide(*it_a, *it_c, *it_b) != iSIDE_POS)
		{
			// Remove point b.
			clist.erase(it_b);

			// Try and move point a back.
			if (it_a != clist.begin())
				--it_a;
		}
		else
		{
			++it_a;

			// Loop is done if it wraps.
			if (it_a == clist.begin())
				return;
		}
	}
}

//**********************************************************************************************
//
template<class T> bool bSimplify
(
	const T& it_a,
	const T& it_b,
	const T& it_c,
	const T& it_d,
	CVector2<>& rv3,
	TReal r_pix_sqr,
	TReal r_min_len,
	CVector2<>& v2_min,
	CVector2<>& v2_max
)
//
// Returns 'true' if the point set should be simplified.
//
//**************************************
{
	// Don't apply simplification to longer line segments.
	if (rLineLengthApprox2D((*it_b), (*it_c)) > r_min_len)
		return false;

	// Find the intersection point.
	if (!bIntersection2D(rv3, *it_a, *it_b, *it_c, *it_d))
		return false;

	if (rv3.tX < v2_min.tX || rv3.tX > v2_max.tX)
		return false;

	if (rv3.tY < v2_min.tY || rv3.tY > v2_max.tY)
		return false;

	// If the area is larger than the threshold, preserve the point.
	TReal r_area = rTriangleArea2D2(rv3, *it_b, *it_c);
	if (r_area > r_pix_sqr)
		return true;
	return false;
}

//**********************************************************************************************
void Simplify(TCList& clist, TReal r_scale, CVector2<> v2_min, CVector2<> v2_max)
{
	// If the list has three points or less, there is no point to a scan.
	if (clist.size() <= 3)
		return;

	// Get the threshold pixel to line area.
	TReal r_pix_sqr = rcsRenderCacheSettings.rPixelsPerArea * (r_scale * r_scale);
	TReal r_min_len = rcsRenderCacheSettings.rPixelsPerLine * r_scale;

	// Create a small border area for the cache.
	{
		CVector2<> v2_d = (v2_max - v2_min) * TReal(0.05);

		// Add a minimum of three pixels to the border.
		SetMinMax(v2_d.tX, 3.0f, 8.0f);
		SetMinMax(v2_d.tY, 3.0f, 8.0f);
		v2_min -= v2_d;
		v2_max += v2_d;
	}

	// Get the beginning of the list.
	TCList::iterator it_a = clist.begin();
	for (;;)
	{
		CVector2<> v2_new;	// Temporary storage for the newly generated point.

		// Get the next three points.
		TCList::iterator it_b = it_a.itNext();
		TCList::iterator it_c = it_b.itNext();
		TCList::iterator it_d = it_c.itNext();

		// Try to replace b and c with the intersection of lines ab and cd.
		if (bSimplify(it_a, it_b, it_c, it_d, v2_new, r_pix_sqr, r_min_len, v2_min, v2_max))
		{
			// Insert the new point.
			clist.insert(it_d, v2_new);

			// Remove points b and c.
			clist.erase(it_b);
			clist.erase(it_c);

			// Try and move point a back.
			if (it_a != clist.begin())
				--it_a;
		}
		else
		{
			++it_a;

			// Loop is done if it wraps.
			if (it_a == clist.begin())
				return;
		}
	}
}
