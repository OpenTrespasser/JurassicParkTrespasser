/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Implementation of plane classes of GeomTypes.cpp.
 *
 * To do:
 *		Figure out whether the clip polygon functions should test for intersection first.
 *		Move guts of clip polygon into non-template function (just calls rEdgeT).
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/GeomTypesPlane.cpp                                       $
 * 
 * 9     97/10/27 20:24 Speter
 * Implemented optimised tf3Box(CTransform3<>&).  esfSideOf functions now call this version of
 * tf3Box().
 * 
 * 8     97/10/23 2:18p Pkeet
 * Removed the 'tf3Box' member function that takes presences and replaced it with a function
 * that takes no parameters and a function that takes a transform.
 * 
 * 7     97/10/10 12:28p Pkeet
 * Maded the 'tf3Box' function accept presence arguments. Added an 'esfSideOf' that worked with
 * a box transform.
 * 
 * 6     97/09/29 16:25 Speter
 * All bounding volumes are now positionless.  Removed v3Pos from Box and Sphere.  Removed
 * v3GetOrigin().  Use CVolSphere for internal sphere intersection code.
 * 
 * 5     97/08/29 23:13 Speter
 * Faster algorithm for intersections with CBoundVolBox; replace esfIsSideOfPoints() with
 * tf3Box().
 * 
 * 4     97/08/28 18:16 Speter
 * Intersecting with camera now always returns esfINTERSECT.
 * 
 * 3     97/06/30 20:15 Speter
 * Added intersection functions for CBoundVolCamera.  Replaced some code with call to
 * esfSideOfSphere.  Made custom bvsGetBoundingSphere() function.  Removed special sphere
 * testing code (now handled in each class's esfIsSideOfPoints).
 * 
 * 2     97-04-21 17:08 Speter
 * Updated for new plane classes.  Improved intersection test with sphere.
 * 
 * 
 * 1     97-04-14 20:39 Speter
 * New file implementing general and specialised plane classes (not needed by all users of
 * CBoundVol).
 * 
 **********************************************************************************************/

#include "Common.hpp"
#include "GeomTypes.hpp"
#include "GeomTypesPriv.hpp"

//**********************************************************************************************
//
// CBoundVolPlane implementation.
//
/*
	//******************************************************************************************
	ESideOf CBoundVolPlane::esfSideOf(const CBoundVolCompound& bvc, 
		const CPresence3<>* ppr3_this, const CPresence3<>* ppr3_it) const
	{
		return bvc.esfSideOfHelp(*this, ppr3_it, ppr3_this);
	}
*/
	//******************************************************************************************
	ESideOf CBoundVolPlane::esfSideOf(const CBoundVolPlane& bvpl, 
		const CPresence3<>* ppr3_this, const CPresence3<>* ppr3_it) const
	{
		// NOT CORRECTLY IMPLEMENTED YET.
//		AlwaysAssert(false);
		return esfINTERSECT;
	}

	//******************************************************************************************
	ESideOf CBoundVolPlane::esfSideOf(const CBoundVolSphere& bvs, 
		const CPresence3<>* ppr3_this, const CPresence3<>* ppr3_it) const
	{
		if (ppr3_this)
		{
			CBoundVolPlane bvpl = *this;
			bvpl *= *ppr3_this;
			return bvpl.esfSideOf(bvs, 0, ppr3_it);
		}

		CVolSphere vs_it(bvs.rRadius, ppr3_it);

		return esfSideOfSphere(vs_it.v3Pos, vs_it.rRadius);
	}
	
	//******************************************************************************************
	ESideOf CBoundVolPlane::esfSideOf(const CTransform3<>& tf3_box) const
	{
		// Intersect with the plane.
		return CPlaneTolerance::esfSideOf(tf3_box);
	}

	//******************************************************************************************
	ESideOf CBoundVolPlane::esfSideOf(const CBoundVolBox& bvb, 
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
	ESideOf CBoundVolPlane::esfSideOf(const CBoundVolPolyhedron& bvp, 
		const CPresence3<>* ppr3_this, const CPresence3<>* ppr3_it) const
	{
		return bvp.esfIsSideOfPoints(*this, ppr3_it, ppr3_this);
	}

	//******************************************************************************************
	ESideOf CBoundVolPlane::esfSideOf(const CBoundVolCamera& bvcam, 
		const CPresence3<>* ppr3_this, const CPresence3<>* ppr3_it) const
	{
		return esfINTERSECT;
	}

