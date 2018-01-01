/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Implementation of GeomTypes.hpp.
 *
 * To do:
 *		Radically speed up intersection tests.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/GeomTypesPoly.cpp                                        $
 * 
 * 21    98/04/11 0:01 Speter
 * Utilise CVolSphere::operator /=().
 * 
 * 20    3/10/98 1:20p Pkeet
 * Added include to "LocalArray.hpp."
 * 
 * 19    97/10/27 20:24 Speter
 * Implemented optimised tf3Box(CTransform3<>&).  esfSideOf functions now call this version of
 * tf3Box().
 * 
 * 18    97/10/23 2:18p Pkeet
 * Removed the 'tf3Box' member function that takes presences and replaced it with a function
 * that takes no parameters and a function that takes a transform.
 * 
 * 17    97/10/10 12:28p Pkeet
 * Maded the 'tf3Box' function accept presence arguments. Added an 'esfSideOf' that worked with
 * a box transform.
 * 
 * 16    97/09/29 16:24 Speter
 * All bounding volumes are now positionless.  Removed v3Pos from Box and Sphere.  Removed
 * v3GetOrigin().  Use CVolSphere for internal sphere intersection code.
 * 
 * 15    97/08/29 23:13 Speter
 * Faster algorithm for intersections with CBoundVolBox; replace esfIsSideOfPoints() with
 * tf3Box().
 * 
 * 14    97/08/28 18:16 Speter
 * Intersecting with camera now always returns esfINTERSECT.
 * 
 * 13    97/08/22 11:17 Speter
 * Changed GetWorldExtents() to take a CTransform3 rather than CPresence3.  
 * 
 * 12    97/07/23 18:07 Speter
 * Replaced CCamera argument to clipping functions with b_perspective flag.   Removed tentative
 * CBoundVolPolygon class.
 * 
 * 11    97/07/16 16:05 Speter
 * CBoundVolPolygon now uses planes of higher tolerance.  Removed commented code.
 * 
 * 10    97/07/07 5:32p Pkeet
 * Replaced 'esfContains' with 'esfSideOf' for tests involving CPlaneT.
 * 
 * 9     97/06/30 20:19 Speter
 * Added intersection functions for CBoundVolCamera.  Replaced some code with calls to
 * esfSideOfSphere and GetWorldExtents.  Replaced vectors for planes and vertices with CAArrays.
 * Removed special sphere testing code (now handled in each class's esfIsSideOfPoints).
 * 
 * 8     97/06/18 3:32p Pkeet
 * Fixed assert.
 * 
 * 7     97/06/10 15:45 Speter
 * Added CBoundVolPolygon, special version of CBoundVolPolyhedron.  Updated code for
 * CPArray::end().
 * 
 * 6     97-05-12 11:21 Speter
 * Implemented CBoundVolPolyhedron::GetWorldExtents, so camera can be added to partitioning.
 * 
 * 5     97-04-22 10:49 Speter
 * Cleaned up functions a little, added esfSideOf() for point arrays (used in intersections).
 * Sped up box/polyhedron intersections a lot.
 * 
 * 4     97-04-21 17:08 Speter
 * Updated for new plane classes.
 * 
 * 3     97-04-14 20:27 Speter
 * Substantially reworked all CBoundVol classes.  Now use esfSideOf() function to return a flag
 * indicating containment and/or intersection.
 * Removed old global intersection tests, replaced with member functions.
 * Implemented more exact intersection functions.
 * 
 * 2     97/03/14 5:29p Pkeet
 * Added the 'GetWorldExtents' member function.
 * 
 * 1     97/03/05 11:18a Pkeet
 * Initial implementation.
 * 
 **********************************************************************************************/

#include "Common.hpp"
#include "GeomTypes.hpp"
#include "GeomTypesPriv.hpp"
#include "Clip.hpp"
#include "Lib/Std/LocalArray.hpp"

#include <memory.h>

//**********************************************************************************************
//
// CBoundVolPolyhedron implementation.
//

	//******************************************************************************************
	CBoundVolPolyhedron::CBoundVolPolyhedron(CPArray<CPlane> papl, CPArray< CVector3<> > pav3) :
		// We are passed an array of ordinary CPlanes.  We can cast this to an array of CPlanePolys,
		// as the only difference is the constant tolerance used in intersections.
		// An assert below verifies size compatibility.
		lplPlanes(reinterpret_cast<TPlaneList&>(papl)), 
		lv3Vertices(pav3), 
		bvsSphere(pav3)
	{
		Assert(sizeof(CPlane) == sizeof(CPlanePoly));

		// Perform consistency checking for convexity.  Assert that all points are on or inside
		// all planes.
//		Assert(esfSideOf(lv3Vertices) == esfINSIDE);
	}

	//******************************************************************************************
	CBoundVolPolyhedron::CBoundVolPolyhedron(const CBoundVolPolyhedron& bvp)
		// Copy constructor duplicates CAArray.
	:	lplPlanes(bvp.lplPlanes.paDup()), 
		lv3Vertices(bvp.lv3Vertices.paDup()), 
		bvsSphere(bvp.bvsSphere)
	{
	}

	//******************************************************************************************
	//
	// Member function implementations.
	//

	//******************************************************************************************
	void CBoundVolPolyhedron::GetWorldExtents(const CTransform3<>& tf3, CVector3<>& rv3_min, CVector3<>& rv3_max) const
	{
		::GetWorldExtents(lv3Vertices, tf3, rv3_min, rv3_max);
	}

	//******************************************************************************************
	CBoundVolPolyhedron& CBoundVolPolyhedron::operator *=(TReal r_scale)
	{
		// Scale the bounding sphere.
		bvsSphere       *= r_scale;

		// Bump the radius up to ensure it still contains the points, even with imprecise calculations.
		bvsSphere.rRadius *= 1.005;

		// Scale the vertices.
		forall (lv3Vertices, TVertexList, it_v3)
		{
			*it_v3 *= r_scale;

			// Ensure that the current vertex is still contained within the sphere.
			Assert(bvsSphere.bContains(*it_v3));
		}

		// Scale the planes.
		forall (lplPlanes, TPlaneList, it_pl)
			*it_pl *= r_scale;

		return *this;
	}

/*
	//******************************************************************************************
	CBoundVol& CBoundVolPolyhedron::operator *=(const CPresence3<>& pr3)
	{
		// Transform the bounding sphere.
		bvsSphere *= pr3;

		// Bump the radius up to ensure it still contains the points, even with imprecise calculations.
		bvsSphere.rRadius *= 1.005;

		// Transform the vertices.
		for (TVertexList::iterator it_v3 = lv3Vertices.begin(); it_v3 != lv3Vertices.end(); it_v3++)
		{
			*it_v3 *= pr3;

			// Ensure the bounding sphere still contains the points.
			Assert(bvsSphere.bContains(*it_v3));
		}

		// Transform the planes.
		for (TPlaneList::iterator it_pl = lplPlanes.begin(); it_pl != lplPlanes.end(); it_pl++)
			*it_pl *= pr3;

		return *this;
	}
*/

	//******************************************************************************************
	ESideOf CBoundVolPolyhedron::esfSideOf(const CVector3<>& v3) const
	{
		// Check each plane. The point is contained by the polyhedron if it lies to the inside
		// of or on each of its planes.
		forall_const (lplPlanes, TPlaneList, it_pl)
		{
			if ((*it_pl).esfSideOf(v3) == esfOUTSIDE)
				return esfOUTSIDE;
		}

		return esfINSIDE;
	}

	//******************************************************************************************
	ESideOf CBoundVolPolyhedron::esfSideOf(CPArray< CVector3<> > pav3_points) const
	{
		ESideOf esf = 0;

		forall_const (lplPlanes, CBoundVolPolyhedron::TPlaneList, it_pl)
		{
			ESideOf esf_plane = (*it_pl).esfSideOf(pav3_points);
			if (esf_plane == esfOUTSIDE)
				return esfOUTSIDE;
			esf |= esf_plane;
		}
			
		return esf;
	}

	//******************************************************************************************
	ESideOf CBoundVolPolyhedron::esfSideOf(const CBoundVolPlane& bvpl, 
		const CPresence3<>* ppr3_this, const CPresence3<>* ppr3_it) const
	{
		// If the volume is esfOUTSIDE, so is the plane.  Otherwise, they intersect.
		return bvpl.esfSideOf(*this, ppr3_it, ppr3_this) == esfOUTSIDE ? esfOUTSIDE : esfINTERSECT;
	}

	//******************************************************************************************
	ESideOf CBoundVolPolyhedron::esfSideOf(const CBoundVolSphere& bvs, 
		const CPresence3<>* ppr3_this, const CPresence3<>* ppr3_it) const
	{
#if 0 && bTEST_SPHERES
		// Check against our bounding sphere first.
		if (bvsGetBoundingSphere().esfSideOf(bvs_it, ppr3_this) == esfOUTSIDE)
			return esfOUTSIDE;
#endif

		// Transform the sphere.
		CVolSphere vs_it(bvs.rRadius, ppr3_it);
		if (ppr3_this)
			vs_it /= *ppr3_this;

		ESideOf esf = 0;

		// Perform a slightly faster version of the general template polyhedron intersection function.
		forall_const (lplPlanes, TPlaneList, it_pl)
		{
			ESideOf esf_pl = (*it_pl).esfSideOfSphere(vs_it.v3Pos, vs_it.rRadius);
			if (esf_pl == esfOUTSIDE)
				// It's outside a plane, therefore outside the volume.
				return esfOUTSIDE;
			esf |= esf_pl;
		}
		return esf;
	}
	
	//******************************************************************************************
	ESideOf CBoundVolPolyhedron::esfSideOf(const CTransform3<>& tf3_box) const
	{
		// Intersect with each plane in turn.
		ESideOf esf = 0;

		forall_const (lplPlanes, CBoundVolPolyhedron::TPlaneList, it_pl)
		{
			ESideOf esf_plane = (*it_pl).esfSideOf(tf3_box);
			if (esf_plane == esfOUTSIDE)
				return esfOUTSIDE;
			esf |= esf_plane;
		}
			
		return esf;
	}

	//******************************************************************************************
	ESideOf CBoundVolPolyhedron::esfSideOf(const CBoundVolBox& bvb, 
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
	ESideOf CBoundVolPolyhedron::esfSideOf(const CBoundVolPolyhedron& bvp, 
		const CPresence3<>* ppr3_this, const CPresence3<>* ppr3_it) const
	{
		return bvp.esfIsSideOfPoints(*this, ppr3_it, ppr3_this);
	}

	//******************************************************************************************
	ESideOf CBoundVolPolyhedron::esfSideOf(const CBoundVolCamera& bvcam, 
		const CPresence3<>* ppr3_this, const CPresence3<>* ppr3_it) const
	{
		return esfINTERSECT;
	}

	//******************************************************************************************
	ESideOf CBoundVolPolyhedron::esfIsSideOfPoints(const CBoundVol& bv,
		const CPresence3<>* ppr3_this, const CPresence3<>* ppr3_it) const
	{
		CPresence3<> pr3_total = pr3Total(ppr3_it, ppr3_this);

		ESideOf esf;
		if (bTestSpheres(bv, *this, &pr3_total, &esf))
			return esf;

		if (ppr3_this || ppr3_it)
		{
			// Transform the points to volume's space.
			CLArray(CVector3<>, pav3, lv3Vertices.uLen);

			CTransform3<> tf3_total = pr3_total;
			for (int i = 0; i < pav3.uLen; i++)
				pav3[i] = lv3Vertices[i] * tf3_total;
			return bv.esfSideOf(pav3);
		}
		else
			// No need to transform.
			return bv.esfSideOf(lv3Vertices);
	}

	//******************************************************************************************
	ESideOf CBoundVolPolyhedron::esfClipPolygonInside(CRenderPolygon& rpoly, CPipelineHeap& plh) const
	{
		//
		// This is pretty simple.  Clip the polygon against each plane in turn, returning
		// if it is clipped away.  
		//
		forall_const (lplPlanes, CBoundVolPolyhedron::TPlaneList, it_pl)
		{
			CClipPlaneT<CPlanePoly> clp(*it_pl);
			if (clp.esfClipPolygonInside(rpoly, plh, false) == esfOUTSIDE)
				return esfOUTSIDE;
		}
			
		return esfINSIDE;
	}


