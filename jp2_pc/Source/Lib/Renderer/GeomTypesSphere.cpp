/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Implementation of GeomTypes.hpp.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/GeomTypesSphere.cpp                                      $
 * 
 * 19    9/03/98 9:34p Pkeet
 * Fixed the 'GetWorldExtents' member function.
 * 
 * 18    8/25/98 5:44p Rvande
 * pointer pickiness MW complains about
 * 
 * 17    98/04/11 0:01 Speter
 * Utilise CVolSphere::operator /=().
 * 
 * 16    97/11/14 4:47p Pkeet
 * Fixed the load constructor and save member function.
 * 
 * 15    97/09/29 16:24 Speter
 * All bounding volumes are now positionless.  Removed v3Pos from Box and Sphere.  Removed
 * v3GetOrigin().  Use CVolSphere for internal sphere intersection code.
 * 
 * 14    97/09/26 16:45 Speter
 * Sped up sphere-box and sphere-poly tests a bit.
 * 
 * 13    97/09/23 7:10p Pkeet
 * Added a load constructor and a save function.
 * 
 * 12    9/05/97 12:47p Mlange
 * Fixed bug in GetWorldExtents() function. It calculated the extents of the sphere incorrectly.
 * 
 * 11    97/08/28 18:16 Speter
 * Intersecting with camera now always returns esfINTERSECT.
 * 
 * 10    97/08/22 11:17 Speter
 * Changed GetWorldExtents() to take a CTransform3 rather than CPresence3.  
 * 
 * 9     97/06/30 20:22 Speter
 * Added intersection functions for CBoundVolCamera.  
 * 
 * 8     97-04-22 10:49 Speter
 * Cleaned up functions a little, added esfSideOf() for point arrays (used in intersections).  
 * 
 * 7     97-04-14 21:23 Speter
 * Oops.  
 * 
 * 6     97-04-14 20:27 Speter
 * Substantially reworked all CBoundVol classes.  Now use esfSideOf() function to return a flag
 * indicating containment and/or intersection.
 * Removed old global intersection tests, replaced with member functions.
 * Implemented more exact intersection functions.
 * 
 * 5     97/03/14 6:29p Pkeet
 * Fixed bug in 'bContainBy' member function for boxes.
 * 
 * 4     97/03/14 5:29p Pkeet
 * Added the 'GetWorldExtents' member function.
 * 
 * 3     97/03/11 4:38p Pkeet
 * Fixed bounding sphere to bounding sphere test.
 * 
 * 2     97/03/08 1:34p Pkeet
 * Modified the sphere - sphere containment. Added sphere - box containment.
 * 
 * 1     97/03/05 11:18a Pkeet
 * Initial implementation.
 * 
 **********************************************************************************************/

#include <math.h>
#include <fcntl.h>
#include <io.h>
#include <stdio.h>

#include "common.hpp"
#include "GeomTypes.hpp"
#include "GeomTypesPriv.hpp"


//**********************************************************************************************
//
// CBoundVolSphere implementation.
//

	//******************************************************************************************
	CBoundVolSphere::CBoundVolSphere(int i_handle)
	{
		int i_num_read;

		// Store the type of bounding volume represented.
		i_num_read = _read(i_handle, (char*)&rRadius, (int)sizeof(rRadius));
		Assert(i_num_read == sizeof(rRadius));
	}

	//******************************************************************************************
	CBoundVolSphere::CBoundVolSphere(CPArray< CVector3<> > pav3)
	{
		// Find largest distance of point from origin.
		TReal r_dist_sqr = 0;

		for (int i = 0; i < pav3.uLen; i++)
			SetMax(r_dist_sqr, pav3[i].tLenSqr());
		rRadius = sqrt(r_dist_sqr) * 1.005;
		
/*
		//
		// Find the pair of points that have the maximal distance between them.
		//
		CVector3<> v3_max_a(0,0,0);
		CVector3<> v3_max_b(0,0,0);

		TReal r_max_sqr_diameter = -1;	// Greatest squared distance found so far.

		int i_index_curr, i_index_match;

		// Iterate through the first half of the set of points. Compare each of these with all the
		// other points in the set and remember the pair with the greatest distance between them.
		for (i_index_curr = 0; i_index_curr < (int)pav3.uLen / 2; i_index_curr++)
		{
			CVector3<> v3_curr = pav3[i_index_curr];

			for (i_index_match = 0; i_index_match < (int)pav3.uLen; i_index_match++)
			{
				CVector3<> v3_match = pav3[i_index_match];

				TReal r_curr_sqr_diameter = (v3_curr - v3_match).tLenSqr();

				if (r_curr_sqr_diameter > r_max_sqr_diameter)
				{
					v3_max_a = v3_curr;
					v3_max_b = v3_match;

					r_max_sqr_diameter = r_curr_sqr_diameter;
				}
			}
		}

		// The centre of the sphere lies between the two points.
		v3Pos   = (v3_max_b - v3_max_a) / 2 + v3_max_a;
		rRadius = sqrt(r_max_sqr_diameter) / 2;

		// The algorithm used here will only find the approximate bounding sphere. Some points may still lie outside it,
		// so we must now iterate through all the points once more and grow the sphere if we find a point not enclosed by it.
		for (i_index_curr = 0; i_index_curr < (int)pav3.uLen; i_index_curr++)
		{
			if (!bContains(pav3[i_index_curr]))
				// The point lies outside, grow the sphere to accomodate it.
				rRadius = (pav3[i_index_curr] - v3Pos).tLen();
		}

		// Ensure all the given data points are contained within the sphere. We allow for a 1/2 percent deviation by
		// creating a new sphere that is slightly larger than this one and checking with it.
		#if VER_DEBUG
			CBoundVolSphere bvs_check(*this);

			bvs_check *= 1.005;

			for (i_index_curr = 0; i_index_curr < (int)pav3.uLen; i_index_curr++)
				Assert(bvs_check.bContains(pav3[i_index_curr]));
		#endif
*/
	}


	//******************************************************************************************
	//
	// Member function implementations.
	//

	//******************************************************************************************
	void CBoundVolSphere::GetWorldExtents(const CTransform3<>& tf3, CVector3<>& rv3_min, CVector3<>& rv3_max) const
	{
		// Dummy point.
		CVector3<> v3 = CVector3<>(0.0f, 0.0f, rRadius);
		v3 = v3 * tf3.mx3Mat;

		// Get the radius of the sphere in world coordinates.
		float f_world_rad = v3.tLen();

		// Set the origin based on the matrix.
		rv3_min = rv3_max = tf3.v3Pos;

		// Extend the points by the world radius of the sphere to get the min and max world values.
		rv3_min.tX -= f_world_rad;
		rv3_min.tY -= f_world_rad;
		rv3_min.tZ -= f_world_rad;
		rv3_max.tX += f_world_rad;
		rv3_max.tY += f_world_rad;
		rv3_max.tZ += f_world_rad;
	}

	//******************************************************************************************
	ESideOf CBoundVolSphere::esfSideOf(const CVector3<>& v3) const
	{
		return v3.tLenSqr() <= rRadius * rRadius ? esfINSIDE : esfOUTSIDE;
	}

	//******************************************************************************************
	ESideOf CBoundVolSphere::esfSideOf(CPArray< CVector3<> > pav3_solid) const
	{
		//
		// This function should not be used, as a solid's relationship to a sphere cannot be
		// determined by combining the relationship of its points; they could all be outside
		// the sphere, and yet the solid could contain the sphere, not be outside it.
		//
		return AlwaysVerify(0);
	}

	//******************************************************************************************
	ESideOf CBoundVolSphere::esfSideOf(const CBoundVolPlane& bvpl, 
		const CPresence3<>* ppr3_this, const CPresence3<>* ppr3_it) const
	{
		// If the volume is esfOUTSIDE, so is the plane.  Otherwise, they intersect.
		return bvpl.esfSideOf(*this, ppr3_it, ppr3_this) == esfOUTSIDE ? esfOUTSIDE : esfINTERSECT;
	}

	//******************************************************************************************
	ESideOf CBoundVolSphere::esfSideOf(const CBoundVolSphere& bvs, 
		const CPresence3<>* ppr3_this, const CPresence3<>* ppr3_it) const
	{
		CVolSphere vs_this(rRadius, ppr3_this);
		CVolSphere vs_it(bvs.rRadius, ppr3_it);

		TReal r_sqr_dist = (vs_this.v3Pos - vs_it.v3Pos).tLenSqr();

		TReal r_sum_radius =  vs_this.rRadius + vs_it.rRadius;
		if (r_sqr_dist > r_sum_radius * r_sum_radius)
			return esfOUTSIDE;
		TReal r_diff_radius = vs_this.rRadius - vs_it.rRadius;
		if (r_diff_radius >= 0 && r_sqr_dist < r_diff_radius * r_diff_radius)
			return esfINSIDE;
		return esfINTERSECT;
	}


	//******************************************************************************************
	ESideOf CBoundVolSphere::esfSideOf(const CBoundVolBox& bvb, 
		const CPresence3<>* ppr3_this, const CPresence3<>* ppr3_it) const
	{
#if 1 || bTEST_SPHERES
		// Do a quick test of box's sphere.
		ESideOf esf_sphere = esfSideOf(bvb.bvsGetBoundingSphere(), ppr3_this, ppr3_it);
		if (esf_sphere != esfINTERSECT)
			return esf_sphere;
#endif
		// Test sphere's relationship to box.
		ESideOf esf = bvb.esfSideOf(*this, ppr3_it, ppr3_this);
		if (esf == esfOUTSIDE)
			// Outsideness is commutative.
			return esfOUTSIDE;

		if (esf == esfINSIDE)
			// If sphere is inside box, box intersects sphere.
			return esfINTERSECT;

		// Transform this sphere to match.
		CVolSphere vs_this(rRadius, ppr3_this);
		if (ppr3_it)
			vs_this /= *ppr3_it;

		// The sphere intersects the box, but may contain it.
		// Determine if any point is outside the sphere.
		// If so, the box intersects the sphere; otherwise, it's inside.
		TReal r_radius_sqr = Sqr(vs_this.rRadius);
		for (uint u_corner = 0; u_corner < 8; u_corner++)
		{
			if ((bvb[u_corner] - vs_this.v3Pos).tLenSqr() > r_radius_sqr)
				return esfINTERSECT;
		}

		return esfINSIDE;
	}

	//******************************************************************************************
	ESideOf CBoundVolSphere::esfSideOf(const CBoundVolPolyhedron& bvp, 
		const CPresence3<>* ppr3_this, const CPresence3<>* ppr3_it) const
	{
#if 1 || bTEST_SPHERES
		// Do a quick test of polyhedron's sphere.
		ESideOf esf_sphere = esfSideOf(bvp.bvsGetBoundingSphere(), ppr3_this, ppr3_it);
		if (esf_sphere != esfINTERSECT)
			return esf_sphere;
#endif
		// Test sphere's relationship to poly.
		ESideOf esf = bvp.esfSideOf(*this, ppr3_it, ppr3_this);
		if (esf == esfOUTSIDE)
			// Outsideness is commutative.
			return esfOUTSIDE;
		else if (esf == esfINSIDE)
			// If sphere is inside poly, poly intersects sphere.
			return esfINTERSECT;
		else
		{
			// Transform this sphere to match.
			CVolSphere vs_this(rRadius, ppr3_this);
			if (ppr3_it)
				vs_this /= *ppr3_it;

			// The sphere intersects the poly, but may contain it.
			// Determine if any point is outside the sphere.
			// If so, the poly intersects the sphere; otherwise, it's inside.
			TReal r_radius_sqr = Sqr(vs_this.rRadius);
			forall_const (bvp.lv3Vertices, CBoundVolPolyhedron::TVertexList, it_v3)
			{
				if ((*it_v3 - vs_this.v3Pos).tLenSqr() > r_radius_sqr)
					return esfINTERSECT;
			}
		}

		return esfINSIDE;
	}

	//******************************************************************************************
	ESideOf CBoundVolSphere::esfSideOf(const CBoundVolCamera& bvcam, 
		const CPresence3<>* ppr3_this, const CPresence3<>* ppr3_it) const
	{
		return esfINTERSECT;
	}
	
	//******************************************************************************************
	void CBoundVolSphere::Save(int i_handle) const
	{
		int i_num_written;

		// Call the base class save function.
		CBoundVol::Save(i_handle);

		// Store the type of bounding volume represented.
		i_num_written = _write(i_handle, (char*)&rRadius, (int)sizeof(rRadius));
		AlwaysAssert(i_num_written == sizeof(rRadius));
	}