/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Implementation of CBoundVolCamera.
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/GeomTypesCamera.cpp                                      $
 * 
 * 16    98/04/11 0:01 Speter
 * Utilise CVolSphere::operator /=().
 * 
 * 15    97.12.08 5:50p Mmouni
 * Changed seteocOutCodes to calculate clipping codes without any branching.
 * 
 * 14    97/10/27 20:24 Speter
 * Implemented optimised tf3Box(CTransform3<>&).  esfSideOf functions now call this version of
 * tf3Box().
 * 
 * 13    97/10/23 2:18p Pkeet
 * Removed the 'tf3Box' member function that takes presences and replaced it with a function
 * that takes no parameters and a function that takes a transform.
 * 
 * 12    97/10/10 12:28p Pkeet
 * Maded the 'tf3Box' function accept presence arguments. Added an 'esfSideOf' that worked with
 * a box transform.
 * 
 * 11    97/09/29 16:23 Speter
 * All bounding volumes are now positionless.  Removed v3Pos from Box and Sphere.  Removed
 * v3GetOrigin().  Use CVolSphere for internal sphere intersection code.
 * 
 * 10    97/09/16 15:53 Speter
 * Sped up outcodes by removing Abs() from diagonal plane tolerance.
 * 
 * 9     97/08/29 23:12 Speter
 * Faster algorithm for intersections with CBoundVolBox; replace esfIsSideOfPoints() with
 * tf3Box().
 * 
 * 8     97/08/28 18:14 Speter
 * Removed world-extent behaviour; now effectively infinite when other volumes attempt to
 * contain cameras.  Optimised sphere/diagonal plane test.  Implemented rDistanceRel() for
 * diagonal planes.
 * 
 * 7     97/08/22 11:20 Speter
 * Use CLArray in CBoundVolCameraT::esfSideOfPoints.  Changed GetWorldExtents() to take
 * CTransform3 instead of CPresence3.  Fully implemented CClipPlaneNull, and
 * CCameraDefParallelOpen.
 * 
 * 6     97/07/23 18:08 Speter
 * Replaced CCamera argument to clipping functions with b_perspective flag.   
 * 
 * 5     97/07/16 16:06 Speter
 * Simplified calls to esfClipPolygonInside, removing intermediate inline function.  Now use
 * same tolerance for all camera planes, including near and far.
 * 
 * 4     97/07/07 5:32p Pkeet
 * Replaced 'esfContains' with 'esfSideOf' for tests involving CPlaneT.
 * 
 * 3     97/07/03 17:17 Speter
 * Restored near clipping plane in perspective camera (needed for render cache, which has a
 * "far" near clipping plane).
 * 
 * 2     97/06/30 20:28 Speter
 * Code moved from ClipCamera.cpp.  CClipVolumeCamera is now CBoundVolCamera.  Implemented
 * CBoundVol functions.  Implemented derived classes as template functions.
 * 
 * 1     97/06/23 21:35 Speter
 * 
 * 5     97/06/10 15:47 Speter
 * Now takes camera as an argument (for new vertex projection).
 * 
 * 4     97/05/23 6:25p Pkeet
 * Untemplatized CPlaneDef.
 * 
 * 3     97-05-08 17:46 Speter
 * Changed near and far plane definitions to have tolerance relative to Y distance, fixing near
 * clip bug.  Changed CClipVolumeCamera::esfSideOf to use esfSideOf rather than bContains.
 * Added asserts to esfClipPolygonInside.
 * 
 * 2     97-05-06 16:06 Speter
 * Made CClipVolumeCamera an abstract base class.  Added named constructor functions for
 * parallel and perspective camera volumes.  Moved diagonal plane definitions to .cpp file.
 * 
 * 1     97-04-21 17:15 Speter
 * New file implementing camera volume clipping.
 * 
 * 2     97-04-08 14:48 Speter
 * 
 * 1     97-04-07 17:57 Speter
 * 
 **********************************************************************************************/

#include "common.hpp"
#include "GeomTypes.hpp"
#include "GeomTypesPriv.hpp"
#include "Clip.hpp"
#include "Lib/GeomDBase/PlaneAxis.hpp"

//**********************************************************************************************
//
template<class C> class CBoundVolCameraT: public CBoundVolCamera, public C
//
// Prefix: bvcam
//
// Implement camera clipping given a camera plane definition class C.
// C must contain CClipPlanes clpLeft, clpRight, clpUp, clpDown, clpFar, and clpNear.
// The definitions of the planes can be such that the volume exists either in real space,
// or normalised camera space.  The points and other geometry fed to these functions must
// match the expected space.
//
//**************************************
{
public:

	//******************************************************************************************
	//
	// Constructors
	//

	CBoundVolCameraT()
	{
	}

	CBoundVolCameraT(const C& c)
		: C(c)
	{
	}

	//******************************************************************************************
	//
	// CBoundVol overrides
	//

	//
	// None of the access functions for bounding volumes are implemented for the camera.
	// Cameras are only usefully on the left side of a bv.esfSideOf(bv) expression.
	//

	//******************************************************************************************
	virtual float fMaxExtent() const
	{
		return FLT_MAX;
	}

	//******************************************************************************************
	virtual void GetWorldExtents(const CTransform3<>& tf3, CVector3<>& rv3_min, CVector3<>& rv3_max) const
	{
		AlwaysAssert(0);
	}

	//******************************************************************************************
	virtual ESideOf esfSideOf(const CVector3<>& v3) const
	{
		if (clpLeft. bContains(v3) &&
			clpRight.bContains(v3) &&
			clpDown. bContains(v3) &&
			clpUp.   bContains(v3) &&
			clpNear. bContains(v3) &&
			clpFar.  bContains(v3))
			return esfINSIDE;
		else
			return esfOUTSIDE;
	}

	//******************************************************************************************
	virtual ESideOf esfSideOf(CPArray< CVector3<> > pav3_solid) const
	{
		ESideOf esf = 0;
		ESideOf esf_sub;

		if ((esf_sub = clpLeft.esfSideOf(pav3_solid)) == esfOUTSIDE)
			return esfOUTSIDE;
		esf |= esf_sub;
		if ((esf_sub = clpRight.esfSideOf(pav3_solid)) == esfOUTSIDE)
			return esfOUTSIDE;
		esf |= esf_sub;
		if ((esf_sub = clpDown.esfSideOf(pav3_solid)) == esfOUTSIDE)
			return esfOUTSIDE;
		esf |= esf_sub;
		if ((esf_sub = clpUp.esfSideOf(pav3_solid)) == esfOUTSIDE)
			return esfOUTSIDE;
		esf |= esf_sub;
		if ((esf_sub = clpNear.esfSideOf(pav3_solid)) == esfOUTSIDE)
			return esfOUTSIDE;
		esf |= esf_sub;
		if ((esf_sub = clpFar.esfSideOf(pav3_solid)) == esfOUTSIDE)
			return esfOUTSIDE;
		esf |= esf_sub;

		return esf;
	}

	//******************************************************************************************
	virtual ESideOf esfSideOf(const CBoundVolPlane& bvpl, 
		const CPresence3<>* ppr3_this, const CPresence3<>* ppr3_it) const
	{
		// If the volume is esfOUTSIDE, so is the plane.  Otherwise, they intersect.
		return bvpl.esfSideOf(*this, ppr3_it, ppr3_this) == esfOUTSIDE ? esfOUTSIDE : esfINTERSECT;
	}

	//******************************************************************************************
	virtual ESideOf esfSideOf(const CBoundVolSphere& bvs, 
		const CPresence3<>* ppr3_this, const CPresence3<>* ppr3_it) const
	{
		// Transform the sphere.
		CVolSphere vs_it(bvs.rRadius, ppr3_it);
		if (ppr3_this)
			vs_it /= *ppr3_this;

		ESideOf esf = 0;
		ESideOf esf_sub;

		if ((esf_sub = clpLeft. esfSideOfSphere(vs_it.v3Pos, vs_it.rRadius)) == esfOUTSIDE)
			return esfOUTSIDE;
		esf |= esf_sub;
		if ((esf_sub = clpRight.esfSideOfSphere(vs_it.v3Pos, vs_it.rRadius)) == esfOUTSIDE)
			return esfOUTSIDE;
		esf |= esf_sub;
		if ((esf_sub = clpDown. esfSideOfSphere(vs_it.v3Pos, vs_it.rRadius)) == esfOUTSIDE)
			return esfOUTSIDE;
		esf |= esf_sub;
		if ((esf_sub = clpUp.   esfSideOfSphere(vs_it.v3Pos, vs_it.rRadius)) == esfOUTSIDE)
			return esfOUTSIDE;
		esf |= esf_sub;
		if ((esf_sub = clpNear. esfSideOfSphere(vs_it.v3Pos, vs_it.rRadius)) == esfOUTSIDE)
			return esfOUTSIDE;
		esf |= esf_sub;
		if ((esf_sub = clpFar.  esfSideOfSphere(vs_it.v3Pos, vs_it.rRadius)) == esfOUTSIDE)
			return esfOUTSIDE;
		esf |= esf_sub;

		return esf;
	}
	
	//******************************************************************************************
	virtual ESideOf esfSideOf(const CTransform3<>& tf3_box) const
	{
		// Intersect with each plane in turn.
		ESideOf esf = 0;
		ESideOf esf_sub;

		if ((esf_sub = clpLeft. esfSideOf(tf3_box)) == esfOUTSIDE)
			return esfOUTSIDE;
		esf |= esf_sub;
		if ((esf_sub = clpRight.esfSideOf(tf3_box)) == esfOUTSIDE)
			return esfOUTSIDE;
		esf |= esf_sub;
		if ((esf_sub = clpDown. esfSideOf(tf3_box)) == esfOUTSIDE)
			return esfOUTSIDE;
		esf |= esf_sub;
		if ((esf_sub = clpUp.   esfSideOf(tf3_box)) == esfOUTSIDE)
			return esfOUTSIDE;
		esf |= esf_sub;
		if ((esf_sub = clpNear. esfSideOf(tf3_box)) == esfOUTSIDE)
			return esfOUTSIDE;
		esf |= esf_sub;
		if ((esf_sub = clpFar.  esfSideOf(tf3_box)) == esfOUTSIDE)
			return esfOUTSIDE;
		esf |= esf_sub;

		return esf;
	}

	//******************************************************************************************
	ESideOf esfSideOf(const CBoundVolBox& bvb, 
		const CPresence3<>* ppr3_this, const CPresence3<>* ppr3_it) const
	{
		// Get the box transform, transform by the required presence.
		CTransform3<> tf3_box = ppr3_this || ppr3_it ? 
			bvb.tf3Box(pr3Total(ppr3_this, ppr3_it)) : 
			bvb.tf3Box();

		// Return side of test results.
		return esfSideOf(tf3_box);
	}

	//******************************************************************************************
	ESideOf esfSideOf(const CBoundVolPolyhedron& bvp, 
		const CPresence3<>* ppr3_this, const CPresence3<>* ppr3_it) const
	{
		return bvp.esfIsSideOfPoints(*this, ppr3_it, ppr3_this);
	}

	//******************************************************************************************
	ESideOf esfSideOf(const CBoundVolCamera& bvcam, 
		const CPresence3<>* ppr3_this, const CPresence3<>* ppr3_it) const
	{
		return esfINTERSECT;
	}

	//******************************************************************************************
	CBoundVolCamera& operator *=(TReal r_scale)
	{
		AlwaysAssert(0);
		return *this;
	}

	//******************************************************************************************
	//
	// CBoundVolCamera overrides
	//

	//******************************************************************************************
	CSet<EOutCode> seteocOutCodes(const CVector3<>& v3) const
	{
		CSet<EOutCode> eoc;

		eoc.ConditionalAdd(clpLeft.bNotContained(v3), eocLEFT);
		eoc.ConditionalAdd(clpRight.bNotContained(v3), eocRIGHT);

		eoc.ConditionalAdd(clpDown.bNotContained(v3), eocDOWN);
		eoc.ConditionalAdd(clpUp.bNotContained(v3), eocUP);

		eoc.ConditionalAdd(clpNear.bNotContained(v3), eocNEAR);
		eoc.ConditionalAdd(clpFar.bNotContained(v3), eocFAR);

/*
		if (!clpLeft.bContains(v3))
			eoc += eocLEFT;
		if (!clpRight.bContains(v3))
			eoc += eocRIGHT;

		if (!clpDown.bContains(v3))
			eoc += eocDOWN;
		if (!clpUp.bContains(v3))
			eoc += eocUP;

		if (!clpNear.bContains(v3))
			eoc += eocNEAR;
		if (!clpFar.bContains(v3))
			eoc += eocFAR;
*/

		return eoc;
	}

	//******************************************************************************************
	ESideOf esfClipPolygonInside(CRenderPolygon& rpoly, CPipelineHeap& plh, bool b_perspective,
		CSet<EOutCode> seteoc_poly) const
	{
		//
		// This is pretty simple.  Clip the polygon against each plane in turn, returning
		// if it is clipped away.  
		//
		if (seteoc_poly[eocFAR]   &&   clpFar.esfClipPolygonInside(rpoly, plh, b_perspective) == esfOUTSIDE)
			return esfOUTSIDE;
		if (seteoc_poly[eocNEAR]  &&  clpNear.esfClipPolygonInside(rpoly, plh, b_perspective) == esfOUTSIDE)
			return esfOUTSIDE;
		if (seteoc_poly[eocLEFT]  &&  clpLeft.esfClipPolygonInside(rpoly, plh, b_perspective) == esfOUTSIDE)
			return esfOUTSIDE;
		if (seteoc_poly[eocRIGHT] && clpRight.esfClipPolygonInside(rpoly, plh, b_perspective) == esfOUTSIDE)
			return esfOUTSIDE;
		if (seteoc_poly[eocUP]    &&    clpUp.esfClipPolygonInside(rpoly, plh, b_perspective) == esfOUTSIDE)
			return esfOUTSIDE;
		if (seteoc_poly[eocDOWN]  &&  clpDown.esfClipPolygonInside(rpoly, plh, b_perspective) == esfOUTSIDE)
			return esfOUTSIDE;

		// Everything should be inside at this point.
		#if VER_DEBUG
			for (int i = 0; i < rpoly.paprvPolyVertices.uLen; i++)
				Assert(esfSideOf(rpoly.paprvPolyVertices[i]->v3Cam) != esfOUTSIDE);
		#endif

		return esfINSIDE;
	}
};

//**********************************************************************************************
//
template<class C> class CBoundVolCameraPerspectiveT: public CBoundVolCameraT<C>
//
// Implements the bounding sphere and world extents functions for perspective cameras.
//
//**************************************
{
public:
	//******************************************************************************************
	//
	// Constructors
	//

	CBoundVolCameraPerspectiveT()
	{
	}

	CBoundVolCameraPerspectiveT(const C& c)
		: CBoundVolCameraT<C>(c)
	{
	}

/*
	//******************************************************************************************
	virtual void GetWorldExtents(const CTransform3<>& tf3, CVector3<>& rv3_min, CVector3<>& rv3_max) const
	{
		::GetWorldExtents(PArray(ArrayData(av3Points)), tf3, rv3_min, rv3_max);
	}

	//******************************************************************************************
	virtual CBoundVolPolyhedron bvpPolyhedron() const
	{
		// Allocate point and plane arrays.
		CPArray< CVector3<> > pav3_points = PArray(ArrayData(av3Points)).paDup();

		CPArray<CPlane> papl_bounding(6);

		//
		// Construct a 6-sided polyhedron.
		//
		// pav3_points[0] is not the origin, but rather the origin moved up to the near clipping plane.
		// This is fine for using a set of points to represent the camera, but for the planes, we must
		// construct them using the actual origin.
		// 
		papl_bounding[0] = CPlane(pav3_points[2], pav3_points[1], CVector3<>(0, 0, 0));	// Left clip plane.
		papl_bounding[1] = CPlane(pav3_points[3], pav3_points[4], CVector3<>(0, 0, 0));	// Right clip plane.
		papl_bounding[2] = CPlane(pav3_points[1], pav3_points[3], CVector3<>(0, 0, 0));	// Bottom clip plane.
		papl_bounding[3] = CPlane(pav3_points[4], pav3_points[2], CVector3<>(0, 0, 0));	// Top clip plane.
		papl_bounding[5] = CPlane(-d3YAxis, clpNear.rPos);								// Near clip plane.
		papl_bounding[4] = CPlane( d3YAxis, -clpFar.rPos);								// Far clip plane.

		return CBoundVolPolyhedron(papl_bounding, pav3_points);
	}
*/
};

//
// Specific plane classes, and camera volume classes utilising them.
// We have normalised and unnormalised versions of parallel and perspective cameras,
// with and without near/far clipping.
//



//
// Diagonal planes for perspective camera clipping.  Y is the depth axis, X and Z the
// screen axes.
//

// Constants.
#define fCAMERA_PLANE_TOLERANCE	1e-4

//**********************************************************************************************
//
class CPlaneDefDiagonal
//
// Base class used for diagonal camera clipping planes.
// Tolerance is proportional to Y.
//
//**************************************
{
public:
	CDir2<> d2Normal;					// Normal used for exact distance calculations (e.g. sphere).
	TReal rSlope;						// The slope of the diagonal plane: X/Y, Z/Y, etc.
										// Used for point-distance calculations, which are projected
										// onto the Y = 0 plane.

	//******************************************************************************************
	CPlaneDefDiagonal(TReal r_slope = 1.0)
		: rSlope(r_slope), d2Normal(1.0, r_slope)
	{
	}

	//******************************************************************************************
	//
	// Overrides.
	//

	//******************************************************************************************
	TReal rTolerance(const CVector3<>& v3) const
	{
		return fCAMERA_PLANE_TOLERANCE * v3.tY;
	}

};

//**********************************************************************************************
//
template<class P> class CClipPlaneDiagT: public CClipPlaneT<P>
//
// A plane class which overrides CClipPlaneT<P>::esfSideOfSphere function to be accurate for 
// diagonal planes.
//
//**************************************
{
public:

	//******************************************************************************************
	//
	// Constructors.
	//

	CClipPlaneDiagT()
	{
	}

	CClipPlaneDiagT(const P& p)
		: CClipPlaneT<P>(p)
	{
	}

	//******************************************************************************************
	//
	// Overrides.
	//

	//******************************************************************************************
	ESideOf esfSideOfSphere(const CVector3<>& v3, TReal r_radius) const
	//
	// Override this function (non-virtually) so that it calculates the actual distance of the
	// point from the plane.  The rDistance() function returns a distance projected onto the XZ
	// plane, which is fine for determining which side a point is on, but not for determining 
	// which side a sphere is on.
	//
	//**********************************
	{
		TReal r_dist = rDistanceExact(v3);
		return r_dist < -r_radius ? esfINSIDE
		     : r_dist > +r_radius ? esfOUTSIDE
			 : esfINTERSECT;
	}
};

//**********************************************************************************************
//
class CPlaneDefLeft: public CPlaneDefDiagonal
//
//**************************************
{
public:

	//******************************************************************************************
	CPlaneDefLeft()
	{
	}

	//******************************************************************************************
	CPlaneDefLeft(TReal r_slope)
		: CPlaneDefDiagonal(r_slope)
	{
	}

	//******************************************************************************************
	TReal rDistance(const CVector3<>& v3) const
	{
		// Left plane, out when -X > -Y*slope.
		return v3.tY * rSlope - v3.tX;
	}

	//******************************************************************************************
	TReal rDistanceRel(const CVector3<>& v3_rel) const
	{
		return rDistance(v3_rel);
	}

	//******************************************************************************************
	TReal rDistanceExact(const CVector3<>& v3) const
	{
		return v3.tY * d2Normal.tY - v3.tX * d2Normal.tX;
	}
};


//**********************************************************************************************
//
class CPlaneDefRight: public CPlaneDefDiagonal
//
//**************************************
{
public:

	//******************************************************************************************
	CPlaneDefRight()
	{
	}

	//******************************************************************************************
	CPlaneDefRight(TReal r_slope)
		: CPlaneDefDiagonal(r_slope)
	{
	}

	//******************************************************************************************
	TReal rDistance(const CVector3<>& v3) const
	{
		// Right plane, out when X > Y*slope.
		return v3.tX - v3.tY * rSlope;
	}

	//******************************************************************************************
	TReal rDistanceRel(const CVector3<>& v3_rel) const
	{
		return rDistance(v3_rel);
	}

	//******************************************************************************************
	TReal rDistanceExact(const CVector3<>& v3) const
	{
		return v3.tX * d2Normal.tX - v3.tY * d2Normal.tY;
	}
};


//**********************************************************************************************
//
class CPlaneDefDown: public CPlaneDefDiagonal
//
//**************************************
{
public:

	//******************************************************************************************
	CPlaneDefDown()
	{
	}

	//******************************************************************************************
	CPlaneDefDown(TReal r_slope)
		: CPlaneDefDiagonal(r_slope)
	{
	}

	//******************************************************************************************
	TReal rDistance(const CVector3<>& v3) const
	{
		// Down plane, out when: -Z > -Y*slope.
		return v3.tY * rSlope - v3.tZ;
	}

	//******************************************************************************************
	TReal rDistanceRel(const CVector3<>& v3_rel) const
	{
		return rDistance(v3_rel);
	}

	//******************************************************************************************
	TReal rDistanceExact(const CVector3<>& v3) const
	{
		return v3.tY * d2Normal.tY - v3.tZ * d2Normal.tX;
	}
};

//**********************************************************************************************
//
class CPlaneDefUp: public CPlaneDefDiagonal
//
//**************************************
{
public:

	//******************************************************************************************
	CPlaneDefUp()
	{
	}

	//******************************************************************************************
	CPlaneDefUp(TReal r_slope)
		: CPlaneDefDiagonal(r_slope)
	{
	}

	//******************************************************************************************
	TReal rDistance(const CVector3<>& v3) const
	{
		// Up plane, out when: Z > Y*slope.
		return v3.tZ - v3.tY * rSlope;
	}

	//******************************************************************************************
	TReal rDistanceRel(const CVector3<>& v3_rel) const
	{
		return rDistance(v3_rel);
	}

	//******************************************************************************************
	TReal rDistanceExact(const CVector3<>& v3) const
	{
		return v3.tZ * d2Normal.tX - v3.tY * d2Normal.tY;
	}
};


//**********************************************************************************************
//
class CPlaneDefNear: public CPlaneDefNegY
//
// Near and far planes for perspective camera have tolerances relative to their positions.
//
//**************************************
{
public:

	//******************************************************************************************
	CPlaneDefNear()
	{
	}

	//******************************************************************************************
	CPlaneDefNear(TReal r_pos)
		: CPlaneDefNegY(r_pos)
	{
	}

	//******************************************************************************************
	TReal rTolerance(const CVector3<>& v3) const
	{
		return fCAMERA_PLANE_TOLERANCE * rPos;
	}
};

//**********************************************************************************************
//
class CPlaneDefFar: public CPlaneDefPosY
//
// Near and far planes for perspective camera have tolerances relative to their positions.
//
//**************************************
{
public:

	//******************************************************************************************
	CPlaneDefFar()
	{
	}

	//******************************************************************************************
	CPlaneDefFar(TReal r_pos)
		: CPlaneDefPosY(r_pos)
	{
	}

	//******************************************************************************************
	TReal rTolerance(const CVector3<>& v3) const
	{
		return fCAMERA_PLANE_TOLERANCE * rPos;
	}
};


// Disable "potential divide by 0" warning here, which occurs when CClipPlaneT<CPlaneDefNull>
// instantiates the rEdgeT() function.
#pragma warning(disable: 4723)

//**********************************************************************************************
//
class CPlaneDefNull: public CPlaneDef
//
// Class representing a null (infinite) plane, which all points are inside.
// This is handy for disabling any particular clipping plane in a camera.
//
//**************************************
{
public:

	//******************************************************************************************
	//
	// Overrides.
	//

	//******************************************************************************************
	TReal rDistance(const CVector3<>& v3) const
	{
		// Every point is to inside (negative side).
		return -1.0;
	}

	//******************************************************************************************
	TReal rDistanceRel(const CVector3<>& v3_rel) const
	{
		return rDistance(v3_rel);
	}

	//******************************************************************************************
	TReal rTolerance(const CVector3<>& v3) const
	{
		return 0.0;
	}
};

//**********************************************************************************************
//
class CClipPlaneNull: public CClipPlaneT<CPlaneDefNull>
//
// Class representing a null (infinite) plane, which all points are inside.
// This is handy for disabling any particular clipping plane in a camera.
//
//**************************************
{
public:

	//******************************************************************************************
	//
	// Overrides.
	//

	//******************************************************************************************
	ESideOf esfSideOf(const CVector3<>& v3) const
	{
		return esfINSIDE;
	}

	//******************************************************************************************
	ESideOf esfSideOf(CPArray< CVector3<> > pav3) const
	{
		return esfINSIDE;
	}

	//******************************************************************************************
	ESideOf esfSideOf(const CTransform3<>& tf3_box) const
	{
		return esfINSIDE;
	}

	//******************************************************************************************
	ESideOf esfSideOfSphere(const CVector3<>& v3, TReal r_radius) const
	{
		return esfINSIDE;
	}

	//******************************************************************************************
	TReal rEdgeT(const CVector3<>& v3_0, const CVector3<>& v3_1) const
	{
		// Override this to avoid divide by 0 in template version.
		return 0;
	}
};

//**********************************************************************************************
//
class CCameraDefPerspective
//
// Prefix: bvcam
//
// A volume which implements clipping for a view pyaramid.
// For the non-normalised volume, we ignore the near clipping plane, for faster intersection tests.
//
//**************************************
{
protected:
	// The particular planes used for clipping.
	CClipPlaneDiagT<CPlaneDefLeft>	clpLeft;
	CClipPlaneDiagT<CPlaneDefRight>	clpRight;
	CClipPlaneDiagT<CPlaneDefDown>	clpDown;
	CClipPlaneDiagT<CPlaneDefUp>	clpUp;
	CClipPlaneT<CPlaneDefNear>		clpNear;
	CClipPlaneT<CPlaneDefFar>		clpFar;

public:
	//******************************************************************************************
	//
	// Constructors
	//

	CCameraDefPerspective()
	{
	}

	CCameraDefPerspective
	(
		TReal r_left_slope,
		TReal r_right_slope,
		TReal r_down_slope,
		TReal r_up_slope,
		TReal r_near_dist,					// Near clipping plane distance.
		TReal r_far_dist					// Far clipping plane distance.
	)
		: clpLeft(r_left_slope), clpRight(r_right_slope), 
		clpDown(r_down_slope), clpUp(r_up_slope),
		clpNear(r_near_dist), clpFar(r_far_dist)
	{
		Assert(clpRight.rSlope > clpLeft.rSlope);
		Assert(clpUp.rSlope    > clpDown.rSlope);
		Assert(r_far_dist      > r_near_dist);
		Assert(r_near_dist     >= 0.0);

/*
		// Near the origin.
		av3Points[0] = CVector3<>(0, r_near_dist, 0);

		// 4 far points.
		av3Points[1] = CVector3<>(clpLeft.rSlope  * clpFar.rPos, clpFar.rPos, clpDown.rSlope * clpFar.rPos);
		av3Points[2] = CVector3<>(clpLeft.rSlope  * clpFar.rPos, clpFar.rPos, clpUp.rSlope   * clpFar.rPos);
		av3Points[3] = CVector3<>(clpRight.rSlope * clpFar.rPos, clpFar.rPos, clpDown.rSlope * clpFar.rPos);
		av3Points[4] = CVector3<>(clpRight.rSlope * clpFar.rPos, clpFar.rPos, clpUp.rSlope   * clpFar.rPos);
		// Pass these points to bounding sphere constructor.
		new(&bvsSphere) CBoundVolSphere(PArray(5, av3Points));
*/
	}
};

//**********************************************************************************************
CBoundVolCamera* pbvcamPerspective
(
	TReal r_left_slope,
	TReal r_right_slope,
	TReal r_down_slope,
	TReal r_up_slope,
	TReal r_near_dist,					// Near clipping plane distance.
	TReal r_far_dist					// Far clipping plane distance.
)
{
	return new CBoundVolCameraPerspectiveT<CCameraDefPerspective>
	(
		CCameraDefPerspective(r_left_slope, r_right_slope, r_down_slope, r_up_slope, r_near_dist, r_far_dist)
	);
}

//
// Specific 45-degree planes for normalised camera clipping.  Y is the depth axis, X and Z the
// screen axes.
//

//**********************************************************************************************
//
class CPlaneDefDiagonalNorm
//
// Base class used for diagonal camera clipping planes.
// Tolerance is proportional to Y.
//
//**************************************
{
public:
	TReal rTolerance(const CVector3<>& v3) const
	{
		return fCAMERA_PLANE_TOLERANCE * v3.tY;
	}
};


//**********************************************************************************************
//
class CPlaneDefLeftNorm: public CPlaneDefDiagonalNorm
//
//**************************************
{
public:

	//******************************************************************************************
	TReal rDistance(const CVector3<>& v3) const
	{
		// Left plane, out when -X > Y.
		return - v3.tX - v3.tY;
	}

	//******************************************************************************************
	TReal rDistanceRel(const CVector3<>& v3_rel) const
	{
		return rDistance(v3_rel);
	}
};


//**********************************************************************************************
//
class CPlaneDefRightNorm: public CPlaneDefDiagonalNorm
//
//**************************************
{
public:

	//******************************************************************************************
	TReal rDistance(const CVector3<>& v3) const
	{
		// Right plane, out when X > Y.
		return v3.tX - v3.tY;
	}

	//******************************************************************************************
	TReal rDistanceRel(const CVector3<>& v3_rel) const
	{
		return rDistance(v3_rel);
	}
};


//**********************************************************************************************
//
class CPlaneDefDownNorm: public CPlaneDefDiagonalNorm
//
//**************************************
{
public:

	//******************************************************************************************
	TReal rDistance(const CVector3<>& v3) const
	{
		// Down plane, out when: -Z > Y.
		return - v3.tZ - v3.tY;
	}

	//******************************************************************************************
	TReal rDistanceRel(const CVector3<>& v3_rel) const
	{
		return rDistance(v3_rel);
	}
};

//**********************************************************************************************
//
class CPlaneDefUpNorm: public CPlaneDefDiagonalNorm
//
//**************************************
{
public:

	//******************************************************************************************
	TReal rDistance(const CVector3<>& v3) const
	{
		// Up plane, out when: Z > Y.
		return v3.tZ - v3.tY;
	}

	//******************************************************************************************
	TReal rDistanceRel(const CVector3<>& v3_rel) const
	{
		return rDistance(v3_rel);
	}
};


//**********************************************************************************************
//
class CCameraDefPerspectiveNorm
//
// Prefix: bvcam
//
// A volume which implements clipping for a normalised view pyaramid.
// In this volume, we have a functioning near clipping plane for accurate clipping.
//
//**************************************
{
protected:
	// The particular planes used for clipping.
	CClipPlaneT<CPlaneDefLeftNorm>	clpLeft;
	CClipPlaneT<CPlaneDefRightNorm>	clpRight;
	CClipPlaneT<CPlaneDefDownNorm>	clpDown;
	CClipPlaneT<CPlaneDefUpNorm>	clpUp;
	CClipPlaneT<CPlaneDefNear>		clpNear;
	CClipPlaneT<CPlaneDefFar>		clpFar;

public:
	//******************************************************************************************
	//
	// Constructors
	//

	CCameraDefPerspectiveNorm()
	{
	}

	CCameraDefPerspectiveNorm
	(
		TReal r_near_dist					// Near clipping plane distance.
	)
		: clpNear(r_near_dist), clpFar(1.0)
	{
		Assert(clpNear.rPos > 0.0 && clpNear.rPos < 1.0);
/*
		// Origin.
		av3Points[0] = CVector3<>(0, clpNear.rPos, 0);

		// 4 far points.
		av3Points[1] = CVector3<>(-1.0, 1.0, -1.0);
		av3Points[2] = CVector3<>(-1.0, 1.0,  1.0);
		av3Points[3] = CVector3<>( 1.0, 1.0, -1.0);
		av3Points[4] = CVector3<>( 1.0, 1.0,  1.0);

		// Pass these points to bounding sphere constructor.
		new(&bvsSphere) CBoundVolSphere(PArray(5, av3Points));
*/
	}
};

//**********************************************************************************************
CBoundVolCamera* pbvcamPerspectiveNorm(TReal r_near_dist)
{
	return new CBoundVolCameraPerspectiveT<CCameraDefPerspectiveNorm>
	(
		CCameraDefPerspectiveNorm(r_near_dist)
	);
}

//
// Parallel camera definitions.
//

//**********************************************************************************************
//
class CCameraDefParallel
//
// A volume which implements clipping for a parallel view box.
//
//**************************************
{
protected:
	// The particular planes used for clipping.
	CClipPlaneT<CPlaneDefNegX>	clpLeft;
	CClipPlaneT<CPlaneDefPosX>	clpRight;
	CClipPlaneT<CPlaneDefNegZ>	clpDown;
	CClipPlaneT<CPlaneDefPosZ>	clpUp;
	CClipPlaneT<CPlaneDefNegY>	clpNear;
	CClipPlaneT<CPlaneDefPosY>	clpFar;

	// Diagonal distance, used for sphere conversion.
	TReal						rExtent;

public:
	//******************************************************************************************
	//
	// Constructors
	//

	CCameraDefParallel()
	{
	}

	CCameraDefParallel
	(
		TReal r_neg_x, TReal r_pos_x,
		TReal r_neg_z, TReal r_pos_z,
		TReal r_neg_y, TReal r_pos_y
	)
		: clpLeft(r_neg_x), clpRight(r_pos_x), 
		clpDown(r_neg_z), clpUp(r_pos_z), 
		clpNear(r_neg_y), clpFar(r_pos_y)
	{
		rExtent = CVector3<>
		(
			clpLeft.rPos - clpRight.rPos,
			clpNear.rPos - clpFar.rPos,
			clpDown.rPos - clpUp.rPos
		).tLen();
	}
};

//**********************************************************************************************
//
class CBoundVolCameraParallel: public CBoundVolCameraT<CCameraDefParallel>
//
// Implement bounding sphere and world extent functions for a parallel camera volume.
//
//**************************************
{
public:

	//******************************************************************************************
	//
	// Constructors.
	//

	CBoundVolCameraParallel(const CCameraDefParallel& camdefpar)
		: CBoundVolCameraT<CCameraDefParallel>(camdefpar)
	{
	}
};

//**********************************************************************************************
//
class CCameraDefParallelOpen
//
// A volume which implements clipping for a parallel view box, with no near or far plane.
//
//**************************************
{
protected:
	// The particular planes used for clipping.
	CClipPlaneT<CPlaneDefNegX>	clpLeft;
	CClipPlaneT<CPlaneDefPosX>	clpRight;
	CClipPlaneT<CPlaneDefNegZ>	clpDown;
	CClipPlaneT<CPlaneDefPosZ>	clpUp;
	CClipPlaneNull				clpNear;
	CClipPlaneNull				clpFar;

public:
	//******************************************************************************************
	//
	// Constructors
	//

	CCameraDefParallelOpen()
	{
	}

	CCameraDefParallelOpen
	(
		TReal r_neg_x, TReal r_pos_x,
		TReal r_neg_z, TReal r_pos_z
	) :
		clpLeft(r_neg_x), clpRight(r_pos_x), 
		clpDown(r_neg_z), clpUp(r_pos_z)
	{
	}
};

//**********************************************************************************************
//
class CBoundVolCameraParallelOpen: public CBoundVolCameraT<CCameraDefParallelOpen>
//
// Implement bounding sphere and world extent functions for a parallel camera volume.
//
//**************************************
{
public:

	//******************************************************************************************
	//
	// Constructors.
	//

	CBoundVolCameraParallelOpen(const CCameraDefParallelOpen& camdefparo)
		: CBoundVolCameraT<CCameraDefParallelOpen>(camdefparo)
	{
	}
};

//**********************************************************************************************
//
// Global functions.
//

	//**********************************************************************************************
	CBoundVolCamera* pbvcamParallel
	(
		TReal r_neg_x, TReal r_pos_x,
		TReal r_neg_z, TReal r_pos_z,
		TReal r_neg_y, TReal r_pos_y
	)
	{
		return new CBoundVolCameraParallel
		(
			CCameraDefParallel(r_neg_x, r_pos_x, r_neg_z, r_pos_z, r_neg_y, r_pos_y)
		);
	}

	//**********************************************************************************************
	CBoundVolCamera* pbvcamParallel
	(
		TReal r_neg_x, TReal r_pos_x,
		TReal r_neg_z, TReal r_pos_z
	)
	{
		return new CBoundVolCameraParallelOpen
		(
			CCameraDefParallelOpen(r_neg_x, r_pos_x, r_neg_z, r_pos_z)
		);
	}

	//**********************************************************************************************
	CBoundVolCamera* pbvcamParallelNorm()
	{
		return new CBoundVolCameraParallel(CCameraDefParallel(-1, 1, -1, 1, 0, 1));
	}

