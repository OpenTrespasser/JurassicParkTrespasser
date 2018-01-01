/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Definition of a 3D plane.  For uses of planes in clipping, see Renderer/Clip.hpp.
 *
 *		ESideOf					Enumerated type
 *		CPlaneDef				Definition for general plane.
 *			CPlaneDefTolerance
 *		CPlaneT<P>
 *			CPlane				General plane class.
 *			CPlaneTolerance
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/GeomDBase/Plane.hpp                                               $
 * 
 * 22    8/25/98 11:37a Rvande
 * loop variable scope fixed
 * 
 * 21    98/06/09 21:31 Speter
 * Changed CPlaneTol to use integer template arg rather than float (not supported in some
 * compilers).
 * 
 * 20    97.12.08 5:47p Mmouni
 * Added bNotContained function for quick clipcode computation.
 * 
 * 19    11/05/97 10:20a Gfavor
 * Added FastMake3DX member function
 * 
 * 18    97/11/04 13:32 Speter
 * Upped default plane tolerance, was breaking when clipping huge terrain objects.
 * 
 * 17    10/29/97 8:08p Gfavor
 * Removed ConstructFast function prototype.
 * 
 * 16    97/10/10 6:16p Pkeet
 * Reordered some instructions in the asm block for the side of test using the box transform for
 * slightly more optimal performance.
 * 
 * 15    97/10/10 6:05p Pkeet
 * Initial asm implementation of the side of member function using a box transform.
 * 
 * 14    97/10/10 3:24p Pkeet
 * Removed tolerances from the box transform side of member function.
 * 
 * 13    97/08/28 18:29 Speter
 * Added CPlaneDef::rDistanceRel() function, and box intersection function which uses it.
 * 
 * 12    97/08/22 6:43p Pkeet
 * Reordered a dot product.
 * 
 * 11    97/07/23 18:01 Speter
 * Added unary - member functions.
 * 
 * 10    97/07/22 3:28p Pkeet
 * Added the 'v3Intersection' member function to CPlaneT.
 * 
 * 9     97/07/16 16:09 Speter
 * Added CPlaneDefTol<> and CPlaneTol<> with templated tolerance.  Removed unused 'r_out_adjust'
 * parameter to CPlaneDef constructor.  Corrected bContains function.  Updated comments.
 * 
 * 8     97/07/07 5:32p Pkeet
 * Replaced 'esfContains' with 'esfSideOf' for tests involving CPlaneT.
 * 
 * 7     97/07/07 4:09p Pkeet
 * Added the 'bInside,' 'bOutside' and 'esfSideOf' functions to CPlaneT. 'esfSideOf' uses an
 * array of points.
 * 
 * 6     97/06/30 19:19 Speter
 * Made bContains return true if the point was within a tolerance.  Removed inefficient early
 * loop break in esfContains(CPArray<CVector3<>>).  Added esfSideOfSphere() function.
 * 
 * 5     97/05/23 6:27p Pkeet
 * Untemplatized CPlaneDef. Added and interface for building a plane equation fast.
 * 
 * 4     97/05/21 18:01 Speter
 * Removed inheritance of CPlaneDef from CPlaneDefInterface.  Added Difference() function for
 * CPlane, so Fuzzy() works.
 * 
 * 3     97-04-22 10:47 Speter
 * Added esfContains() function for point arrays, used by bounding volume classes.
 * 
 * 2     97-04-21 16:45 Speter
 * Made CPlaneDef class, for defining basic characteristics of a plane.
 * Made CPlaneT<> template class, for combining common characteristics of all planes.
 * Made new CPlaneTolerance class, for a plane with customised tolerance value.
 * 
 * 1     97-04-14 20:41 Speter
 * Contains code moved from GeomTypes.hpp.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_GEOMDBASE_PLANE_HPP
#define HEADER_LIB_GEOMDBASE_PLANE_HPP

#include "Lib/Transform/Transform.hpp"

#include <float.h>

//
// Clipping plane 'thickness'. A point is considered to lie ON a clipping plane if it lies 
// within this distance from the plane.
// 
// Error analysis:
//		Based on the number of calculations involved in constructing a plane definition from
//		3 points, and then computing the distance to another point, the error should be about
//		50 times the scalar error margin (which is FLT_EPSILON).  So we set our tolerance for
//		this calculation high enough to allow for this.
//

#define fPLANE_TOLERANCE (FLT_EPSILON * 500)

//**********************************************************************************************
//
//
typedef int ESideOf;
//
// Prefix: esf
//
// Indicates which side of a volume a point lies on.
// These values are designed so that the relationship of any point, edge, polygon, or polyhedron
// to the volume can be found by simply oring together the values of its constituents.
//
// This type is in reality an int rather than an enum, to allow the or'ing behaviour.
//
//**************************************
enum
{
	esfON,							// Object is on the plane (to within its defined fuzziness).
									// Note that the value of esfON is 0, so that the combination of esfON 
									// and esfOUTSIDE is just esfOUTSIDE.
	esfINSIDE,						// Object is to the inside.
	esfOUTSIDE,						// Object is to the outside.
	esfINTERSECT					// Object is on both the inside and outside.
									// Happens to be equal to esfINSIDE | esfOUTSIDE.
};

//**********************************************************************************************
//
class CPlaneDefInterface
//
// A class defining the interface for a plane definition class, used as the template parameter
// to CPlane<> below.
//
// Note: Do not inherit from CPlaneDefInterface; you don't need to, because it's just a
// documentation interface.  MSVC 4.2 doesn't like zero-length classes,
// and will lash out at you in a passive-aggressive manner by silently generating bad code.
//
//**************************************
{
public:
	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	TReal rDistance
	(
		const CVector3<>& v3			// The point to measure.
	) const;
	//
	// Returns:
	//		The signed distance of the point to the outside of this plane.  
	//
	// Note that a plane has a certain "thickness" for tolerance purposes, and thus really consists 
	// of an inner and outer plane.  A point is considered inside the plane if it lies to the inside 
	// of the inner plane, outside if outside the outer plane, and on the plane if  between them.
	// 
	// The distance returned is the distance to the outer plane.  Thus, if the distance is positive,
	// the point is outside the plane; if negative, but within the tolerance value of 0, on the plane;
	// and if negative, and less than the negative tolerance value, inside the plane.
	//
	//**************************************

	//******************************************************************************************
	//
	TReal rDistanceRel
	(
		const CVector3<>& v3_rel		// An offset to measure.
	) const;
	//
	// Returns:
	//		The relative signed distance to the outside of this plane increased by the relative
	//		vector.  (This value is the same as rDistance(), except it omits any constant term
	//		in the plane equation).  For example, if a point v3 has distance r_d = rDistance(v3) 
	//		from the plane, then the point (v3 + v3_rel) has distance (r_d + rDistanceRel(v3_rel)).  
	//
	//
	//**********************************

	//******************************************************************************************
	//
	TReal rTolerance
	(
		const CVector3<>& v3				// Point at which tolerance is to be determined
											// (may be ignored if constant tolerance).
	) const;
	//
	// Returns:
	//		The tolerance allowed inside the plane for determining whether a point is on the plane.
	//
	// See rDistance() above.
	//
	//**************************************
};

//**********************************************************************************************
//
class CPlaneDef
//
// A general-purpose plane definition class, with constant tolerance.
//
//**************************************
{
public:
	CDir3<>	d3Normal;	// Surface normal.
	TReal	rD;			// Shortest distance from the plane to the origin, in its positive direction.
						// This is >0 if the plane faces the origin, <0 if it faces away.


	//******************************************************************************************
	//
	// Constructors and destructor.
	//

	// Default constructor.
	CPlaneDef()
	{
	}

	// Initialise with a normal and distance.  Move the plane 
	CPlaneDef(CDir3<> d3, TReal r_d)
		: d3Normal(d3), rD(r_d)
	{
	}

	// Initialise with three points defining a plane. The points are ordered clockwise around
	// the surface normal.
	// NOTE: No checks are made to ensure the points define a plane!
	CPlaneDef
	(
		const CVector3<>& v3_a, const CVector3<>& v3_b, const CVector3<>& v3_c
	);

	//******************************************************************************************
	//
	// Operators.
	//

	//******************************************************************************************
	CPlaneDef operator -() const
	{
		// Return opposite-facing plane.
		return CPlaneDef(-d3Normal, -rD);
	}

	//******************************************************************************************
	CPlaneDef& operator *=(const CPresence3<>& pr3)
	// Transform the plane with the given transform.
	{
		// Rotate the normal by the presence.
		d3Normal *= pr3.r3Rot;

		// Scale the distance by the presence.
		rD *= pr3.rScale;

		// The distance also changes due to movement.
		rD -= pr3.v3Pos * d3Normal;

		return *this;
	}

	//******************************************************************************************
	CPlaneDef& operator *=(TReal r_scale)
	{
		rD *= r_scale;
		return *this;
	}

	//******************************************************************************************
	//
	// Overrides.
	//

	//******************************************************************************************
	TReal rDistance(const CVector3<>& v3) const
	{
		return v3 * d3Normal + rD;
	}

	//******************************************************************************************
	TReal rDistanceRel(const CVector3<>& v3_rel) const
	{
		return v3_rel * d3Normal;
	}

	//******************************************************************************************
	TReal rTolerance(const CVector3<>& v3_rel) const
	{
		// Return the constant.
		return fPLANE_TOLERANCE;
	}

	//******************************************************************************************
	void FastMake
	(
		const CVector3<>& v3_a,
		const CVector3<>& v3_b,
		const CVector3<>& v3_c
	);
	//
	// Builds a less accurate but faster plane equation.
	//
	//**************************************

	//******************************************************************************************
	void FastMake3DX
	(
		const CVector3<>& v3_a,
		const CVector3<>& v3_b,
		const CVector3<>& v3_c
	);
	//
	// Builds a less accurate but faster plane equation.
	//
	//**************************************
};

//**********************************************************************************************
//
template<int I_TOL_RECIP> class CPlaneDefTol: public CPlaneDef
//
// A general-purpose plane definition class, with specified constant tolerance.
//
//**************************************
{
public:
	//******************************************************************************************
	//
	// Constructors and destructor.
	//

	// Default constructor.
	CPlaneDefTol()
	{
	}

	// Initialise with a plane definition.
	CPlaneDefTol(const CPlaneDef& pld)
		: CPlaneDef(pld)
	{
	}

	// Initialise with a normal and distance.
	CPlaneDefTol(CDir3<> d3, TReal r_d)
		: CPlaneDef(d3, r_d)
	{
	}

	// Initialise with three points defining a plane. 
	CPlaneDefTol(const CVector3<>& v3_a, const CVector3<>& v3_b, const CVector3<>& v3_c)
		: CPlaneDef(v3_a, v3_b, v3_c)
	{
	}

	//******************************************************************************************
	//
	// Overrides.
	//

	//******************************************************************************************
	TReal rTolerance(const CVector3<>&) const
	{
		// Return the template parameter.
		return 1.0f / float(I_TOL_RECIP);
	}
};

//**********************************************************************************************
//
class CPlaneDefTolerance: public CPlaneDef
//
// Adds a per-plane tolerance to CPlaneDef.
//
//**************************************
{
private:
	TReal	rTolerance_;

public:
	//******************************************************************************************
	//
	// Constructors.
	//

	// Default constructor.
	CPlaneDefTolerance()
	{
	}

	// Initialise with a plane definition and tolerance.
	CPlaneDefTolerance(const CPlaneDef& pld, TReal r_tolerance = fPLANE_TOLERANCE)
		: CPlaneDef(pld), rTolerance_(r_tolerance)
	{
	}

	// Initialise with a normal, distance, and tolerance.
	CPlaneDefTolerance(CDir3<> d3, TReal r_d, TReal r_tolerance = fPLANE_TOLERANCE)
		: CPlaneDef(d3, r_d), rTolerance_(r_tolerance)
	{
	}

	// Initialise with three points defining a plane. 
	CPlaneDefTolerance
	(
		const CVector3<>& v3_a, const CVector3<>& v3_b, const CVector3<>& v3_c,
		TReal r_tolerance = fPLANE_TOLERANCE
	)
		: CPlaneDef(v3_a, v3_b, v3_c), rTolerance_(r_tolerance)
	{
	}

	//******************************************************************************************
	//
	// Overrides.
	//

	//******************************************************************************************
	TReal rTolerance(const CVector3<>&) const
	{
		return rTolerance_;
	}
};

//**********************************************************************************************
//
template<class P> class CPlaneT: public P
//
// Template class for a general or specialised (e.g. axis-aligned) plane.
// P must implement CPlaneDefInterface.
//
//**************************************
{
public:

	//******************************************************************************************
	//
	// Constructors.
	//

	CPlaneT()
	{
	}

	CPlaneT(const P& p)
		: P(p)
	{
	}

	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	ESideOf esfSideOf
	(
		const CVector3<>& v3			// Point to test.
	) const
	//
	// Returns:
	//		The point's relation to the plane: esfINSIDE if away from the normal, esfOUTSIDE if
	//		toward the normal, esfON if on the plane, to within the plane's tolerance.
	//
	//**********************************
	{
		// Makes use of rDistance().
		TReal r_dist = rDistance(v3);
		return r_dist >  rTolerance(v3) ? esfOUTSIDE :
			   r_dist < -rTolerance(v3) ? esfINSIDE  :
			                              esfON;
	}

	//******************************************************************************************
	//
	bool bContains
	(
		const CVector3<>& v3			// Point to test.
	) const
	//
	// Returns:
	//		Whether the point is inside or on the plane (to within tolerance).
	//
	// Notes:
	//		Faster than also testing for esfON, useful for time-critical code.
	//
	//**********************************
	{
		// Makes use of rDistance().
		return rDistance(v3) <= rTolerance(v3);
	}

	//******************************************************************************************
	//
	bool bNotContained
	(
		const CVector3<>& v3			// Point to test.
	) const
	//
	// Returns:
	//		Whether the point is outside the plane (to within tolerance).
	//
	//**********************************
	{
		// rDistance(v3) > rTolerance(v3)
		return CIntFloat(rTolerance(v3) - rDistance(v3)).bSign();
	}

	//******************************************************************************************
	//
	ESideOf esfSideOfSphere
	(
		const CVector3<>& v3,			// Centre of sphere.
		TReal r_radius					// Its radius.
	) const
	//
	// Returns:
	//		The sphere's relation to the plane: esfINSIDE if away from the normal, esfOUTSIDE if
	//		toward the normal, esfINTERSECT if on the plane.
	//
	//**********************************
	{
		// Makes use of rDistance().
		TReal r_dist = rDistance(v3);
		return r_dist >  r_radius ? esfOUTSIDE :
			   r_dist < -r_radius ? esfINSIDE  :
			                        esfINTERSECT;
	}

	//******************************************************************************************
	//
	ESideOf esfSideOf
	(
		CPArray< CVector3<> > pav3			// Array of points to test.
	) const
	//
	// Returns:
	//		The points' relation to the plane: esfINSIDE if away from the normal, esfOUTSIDE if
	//		toward the normal, esfON if on the plane, to within the plane's tolerance.
	//
	//**********************************
	{
		ESideOf esf = 0;

		// Look for the first point that is not on the plane.
		uint u;
		for (u = 0; (u < pav3.uLen) && !esf; ++u)
			esf |= esfSideOf(pav3[u]);

		// Choose path.
		if (esf == esfINSIDE)
		{
			// Look for outside points only.
			for (; u < pav3.uLen; ++u)
			{
				if (bOutside(pav3[u]))
					return esfINTERSECT;
			}
			return esfINSIDE;
		}

		if (esf == esfOUTSIDE)
		{
			// Look for inside points only.
			for (; u < pav3.uLen; ++u)
			{
				if (bInside(pav3[u]))
					return esfINTERSECT;
			}
			return esfOUTSIDE;
		}

		return esfON;
	}

	//******************************************************************************************
	//
	ESideOf esfSideOf
	(
		const CTransform3<>& tf3_box	// Transform describing a box in 3-space.
										// Specifies the transformation of a unit cube (0..1).
										// The v3Pos field indicates the position of the box origin.
										// The mx3Mat.v3X, etc. fields describe the length/direction
										// of each axis.
	) const
	//
	// Returns:
	//		The box's relation to the plane.
	//
	//**********************************
#if VER_ASM
	{
		//
		// For notes see the non-asm version.
		//

		// Find min and max distances.
		TReal r_dist = rDistance(tf3_box.v3Pos);
		TReal r_dx   = rDistanceRel(tf3_box.mx3Mat.v3X);
		TReal r_dy   = rDistanceRel(tf3_box.mx3Mat.v3Y);
		TReal r_dz   = rDistanceRel(tf3_box.mx3Mat.v3Z);

		__asm
		{
			mov edx, 0x80000000
			mov eax, r_dx

			and edx, r_dist
			jz short DIST_POSITIVE

			xor eax, 0x80000000
			mov ebx, r_dy

			sar eax, 31
			xor ebx, 0x80000000

			sar ebx, 31
			mov ecx, r_dz

			and eax, r_dx
			xor ecx, 0x80000000

			sar ecx, 31
			and ebx, r_dy

			and ecx, r_dz
			mov r_dx, eax

			mov r_dy, ebx

			fld  r_dist

			mov r_dz, ecx

			fld  r_dx
			fadd r_dy
			fxch st(1)
			fadd r_dz
			fadd
			mov edx, 0x80000000
			fstp r_dist

			and edx, r_dist
			jz short RETURN_INTERSECT
		}
		return esfINSIDE;

		DIST_POSITIVE:
		__asm
		{
			sar eax, 31
			mov ebx, r_dy

			sar ebx, 31
			mov ecx, r_dz

			sar ecx, 31
			and eax, r_dx

			and ebx, r_dy
			and ecx, r_dz

			mov r_dx, eax

			fld  r_dist

			mov r_dy, ebx
			mov r_dz, ecx

			fld  r_dx
			fadd r_dy
			fxch st(1)
			fadd r_dz
			fadd
			mov edx, 0x80000000
			fstp r_dist

			and edx, r_dist
			jnz short RETURN_INTERSECT
		}
		return esfOUTSIDE;

	RETURN_INTERSECT:
		return esfINTERSECT;
	}
#else
	{
		// Find min and max distances.
		TReal r_dist = rDistance(tf3_box.v3Pos);
		TReal r_dx   = rDistanceRel(tf3_box.mx3Mat.v3X);
		TReal r_dy   = rDistanceRel(tf3_box.mx3Mat.v3Y);
		TReal r_dz   = rDistanceRel(tf3_box.mx3Mat.v3Z);

		if (r_dist < 0.0f)
		{
			// Base is on or inside; consider the bos inside unless any other points are outside.
			// (Note: if the base point is ON the plane, and all other points are OUTSIDE, this
			// will consider the box as INTERSECTING rather than OUTSIDE).
			// Find maximum distance.
			if (r_dx > 0.0f)
				r_dist += r_dx;
			if (r_dy > 0.0f)
				r_dist += r_dy;
			if (r_dz > 0.0f)
				r_dist += r_dz;

			if (r_dist > 0.0f)
				return esfINTERSECT;
			else
				return esfINSIDE;
		}
		else
		{
			// Base is outside; consider the box outside unless any points are inside.
			// Find minimum distance.
			if (r_dx < 0.0f)
				r_dist += r_dx;
			if (r_dy < 0.0f)
				r_dist += r_dy;
			if (r_dz < 0.0f)
				r_dist += r_dz;

			if (r_dist < 0.0f)
				return esfINTERSECT;
			else
				return esfOUTSIDE;
		}
	}
#endif // VER_ASM

	//******************************************************************************************
	//
	CVector3<> v3Intersection
	(
		const CVector3<>& v3_0,	// Vectors describing edge to intersect.
		const CVector3<>& v3_1
	) const
	//
	// Returns:
	//		The point of intersection between the edge and the bounding volume.
	//
	//**********************************
	{
		// Find the parametric value t.
		TReal r_0 = rDistance(v3_0);
		TReal r_1 = rDistance(v3_1);

		// If the two points have the same distance they form a line parallel to the plane.
		if (r_0 == r_1)
			return v3_0;
		TReal r_t = r_0 / (r_0 - r_1);

		// Construct and return a point generated from the interpolation.
		return v3_0 * (TReal(1.0) - r_t) + v3_1 * r_t;
	}

protected:

	//******************************************************************************************
	//
	bool bInside
	(
		const CVector3<>& v3	// Point to test.
	) const
	//
	// Returns 'true' if the point is strictly inside the plane.
	//
	// Notes:
	//		Faster than also testing for esfON and esfINSIDE, useful for time-critical code.
	//
	//**********************************
	{
		// Makes use of rDistance().
		return rDistance(v3) < -rTolerance(v3);
	}

	//******************************************************************************************
	//
	bool bOutside
	(
		const CVector3<>& v3	// Point to test.
	) const
	//
	// Returns 'true' if the point is strictly outside the plane.
	//
	// Notes:
	//		Faster than also testing for esfON and esfINSIDE, useful for time-critical code.
	//
	//**********************************
	{
		// Makes use of rDistance().
		return rDistance(v3) > rTolerance(v3);
	}
};

//**********************************************************************************************
//
class CPlane: public CPlaneT<CPlaneDef>
//
// A plane represented by a surface normal vector and the shortest distance from the plane to
// the origin.
//
// Prefix: pl
//
//**************************************
{
public:
	//******************************************************************************************
	//
	// Constructors and destructor.
	//

	// Default constructor.
	CPlane()
	{
	}

	CPlane(CDir3<> d3, TReal r_d)
		: CPlaneT<CPlaneDef>(CPlaneDef(d3, r_d))
	{
	}

	CPlane(const CVector3<>& v3_a, const CVector3<>& v3_b, const CVector3<>& v3_c)
		: CPlaneT<CPlaneDef>(CPlaneDef(v3_a, v3_b, v3_c))
	{
	}

	//******************************************************************************************
	//
	// Operators.
	//

	CPlane operator -() const
	{
		return CPlane(-d3Normal, -rD);
	}
};

//
// Fuzzy specialisation.
//


//**********************************************************************************************
inline TReal Difference(const CPlane& pl_a, const CPlane& pl_b)
//
// Specialise the Difference function used by CFuzzy<>.
//
// Note: this version does not allow ordering comparisions, just [in]equality.
//
//**************************************
{
	return Abs(pl_a.d3Normal.tX - pl_b.d3Normal.tX) + 
		   Abs(pl_a.d3Normal.tY - pl_b.d3Normal.tY) + 
		   Abs(pl_a.d3Normal.tZ - pl_b.d3Normal.tZ) +
		   Abs(pl_a.rD          - pl_b.rD);
}

inline CFuzzy<CPlane, TReal> Fuzzy(const CPlane& t_value, TReal r_tolerance = 0.001)
// Specialise the Fuzzy function for tolerance.
{
	return CFuzzy<CPlane, TReal>(t_value, r_tolerance);
}

//**********************************************************************************************
//
template<int I_TOL_RECIP> class CPlaneTol: public CPlaneT< CPlaneDefTol<I_TOL_RECIP> >
//
// Like CPlane, but with templated tolerance.
//
//**************************************
{
public:
	//******************************************************************************************
	//
	// Constructors and destructor.
	//

	// Default constructor.
	CPlaneTol()
	{
	}

	CPlaneTol(const CPlane& pl)
		: CPlaneT< CPlaneDefTol<I_TOL_RECIP> >(CPlaneDefTol<I_TOL_RECIP>(pl))
	{
	}
};

//**********************************************************************************************
//
class CPlaneTolerance: public CPlaneT<CPlaneDefTolerance>
//
// Like CPlane, but with customisable tolerance.
//
//**************************************
{
public:
	//******************************************************************************************
	//
	// Constructors and destructor.
	//

	// Default constructor.
	CPlaneTolerance()
	{
	}

	CPlaneTolerance(const CPlane& pl, TReal r_tolerance = fPLANE_TOLERANCE)
		: CPlaneT<CPlaneDefTolerance>(CPlaneDefTolerance(pl, r_tolerance))
	{
	}
};


#endif
