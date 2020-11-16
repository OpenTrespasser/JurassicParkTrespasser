/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Implementation of GeomTypes.hpp.
 *
 * To do:
 *		Drastically speed up intersection test.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/GeomTypesBox.cpp                                         $
 * 
 * 44    98/10/04 23:02 Speter
 * Nearest-point selection now takes account of inward adjustment done to point.
 * 
 * 43    98/09/30 18:58 Speter
 * Made CBoundVolBox constructors inline.
 * 
 * 42    98/09/28 2:46 Speter
 * Nearest-point raycast no longer checks faces on opposite side of cast point; prevents hand
 * from making stupid choices.
 * 
 * 41    9/24/98 6:03p Asouth
 * stupid programmer error
 * 
 * 40    98/09/12 0:53 Speter
 * Improved nearest-point raycast; provides better logic for hand pickup.
 * 
 * 39    8/25/98 5:41p Rvande
 * fixed picky pointer types and 'that' usage for MWerks
 * 
 * 38    98/07/19 17:49 Speter
 * Removed unneeded includes.
 * 
 * 37    98/06/17 16:49 Speter
 * Fixed bugs with "nearest point" raycast: calculated wrong distance, didn't scale it, and
 * chose inappropriate face.
 * 
 * 36    98/05/25 1:23 Speter
 * Fixed inside box-pickup bug. Remember, cut, paste, then MODIFY.
 * 
 * 35    98/05/15 14:05 Speter
 * Added optimisation for aligned box intersection.
 * 
 * 34    98/05/06 23:18 Speter
 * If r_length == 0, find nearest point in any direction.
 * 
 * 33    98/04/17 17:57 Speter
 * Fixed bRayIntersect. Removed bCollide (obsoleted by bRayIntersect).
 * 
 * 32    98/04/16 14:14 Speter
 * Added bRayIntersect() to bounding volumes, implemented in CBoundVolBox. Eventually will
 * replace bCollide().
 * 
 * 31    98/04/14 14:40 Speter
 * Simplified, sped up bCollide.
 * 
 * 30    98/04/13 23:19 Speter
 * Fixed bCollide() function to return nearest collision.
 * 
 * 29    98/04/11 0:02 Speter
 * In Box/Box test, do Box/Sphere test first, then Sphere/Box test to detect some false
 * intersects. Utilise CVolSphere::operator /=().
 * 
 * 28    98/04/06 19:51 Speter
 * Added sphere-sphere prelim test to box-box test.
 * 
 * 27    3/10/98 1:21p Pkeet
 * Added include to "LocalArray.hpp."
 * 
 * 26    3/06/98 9:16p Agrant
 * clamp function to force a point to be within the bounding volume
 * 
 * 25    97/12/05 21:11 Speter
 * Made some functions more efficient, using 'that' to reduce copying.
 * 
 * 24    97/11/14 4:47p Pkeet
 * Fixed the load constructor and save member function.
 * 
 * 23    11/03/97 3:33p Agrant
 * Minimum bounding box sizes, to prevent zero volumes.
 * 
 * 22    97/10/27 20:24 Speter
 * Implemented optimised tf3Box(CTransform3<>&).  esfSideOf functions now call this version of
 * tf3Box().
 * 
 * 21    97/10/23 2:18p Pkeet
 * Removed the 'tf3Box' member function that takes presences and replaced it with a function
 * that takes no parameters and a function that takes a transform.
 * 
 * 20    97/10/15 6:45p Pkeet
 * Changed 'v3Collide' to 'bCollide.'
 * 
 * 19    97/10/15 1:29a Pkeet
 * Added the 'v3Collide' member function.
 * 
 * 18    97/10/10 12:28p Pkeet
 * Maded the 'tf3Box' function accept presence arguments. Added an 'esfSideOf' that worked with
 * a box transform.
 * 
 **********************************************************************************************/

#include "common.hpp"
#include "GeomTypes.hpp"
#include "GeomTypesPriv.hpp"
#include "Lib/GeomDBase/PlaneAxis.hpp"
#include "Lib/Std/LocalArray.hpp"

#include <io.h>

//**********************************************************************************************
//
// CBoundVolBox implementation.
//

	//******************************************************************************************
	CBoundVolBox::CBoundVolBox(int i_handle)
	{
		int i_num_read;

		// Store the type of bounding volume represented.
		i_num_read = _read(i_handle, (char*)&v3Max, (int)sizeof(v3Max));
		Assert(i_num_read == sizeof(v3Max));	
		i_num_read = _read(i_handle, (char*)&rExtent, (int)sizeof(rExtent));
		Assert(i_num_read == sizeof(rExtent));	
	}

	//******************************************************************************************
	CBoundVolBox::CBoundVolBox(CPArray< CVector3<> > pav3)
	{
		// Calculate max corner point and max radius separately (max radius might be smaller).
		TReal r_dist_sqr = 0;

		// Make sure we are at least a minimum size.
		const TReal r_min_dimension = .01;
		v3Max = CVector3<>(r_min_dimension, r_min_dimension, r_min_dimension);
		r_dist_sqr = v3Max.tLenSqr();

		// Now make sure every point is inside the box.
		for (int i_index = 0; i_index < pav3.uLen; i_index++)
		{
			SetMax(v3Max.tX, Abs(pav3[i_index].tX));
			SetMax(v3Max.tY, Abs(pav3[i_index].tY));
			SetMax(v3Max.tZ, Abs(pav3[i_index].tZ));
			SetMax(r_dist_sqr, pav3[i_index].tLenSqr());
		}

		rExtent = sqrt(r_dist_sqr) * 1.005;
	}

	//******************************************************************************************
	CVector3<> CBoundVolBox::operator [](uint u_corner) const
	{
		// For some bizarre reason, index 0 references the high coord, and 7 the low one.
		switch (u_corner)
		{
			default:
				Assert(0);
			case 0:
				return v3Max;
			case 1:
				return CVector3<>(-v3Max.tX, v3Max.tY, v3Max.tZ);
			case 2:
				return CVector3<>(v3Max.tX, -v3Max.tY, v3Max.tZ);
			case 3:
				return CVector3<>(-v3Max.tX, -v3Max.tY, v3Max.tZ);
			case 4:
				return CVector3<>(v3Max.tX, v3Max.tY, -v3Max.tZ);
			case 5:
				return CVector3<>(-v3Max.tX, v3Max.tY, -v3Max.tZ);
			case 6:
				return CVector3<>(v3Max.tX, -v3Max.tY, -v3Max.tZ);
			case 7:
				return -v3Max;
		}
	}

	//******************************************************************************************
	TReal CBoundVolBox::operator [](EBoxExtents ebe) const
	{
		switch (ebe)
		{
			default:
				Assert(false);
			case ebeMIN_X:
				return -v3Max.tX;
			case ebeMAX_X:
				return v3Max.tX;
			case ebeMIN_Y:
				return -v3Max.tY;
			case ebeMAX_Y:
				return v3Max.tY;
			case ebeMIN_Z:
				return -v3Max.tZ;
			case ebeMAX_Z:
				return v3Max.tZ;
		}
	}

	//******************************************************************************************
	void CBoundVolBox::GetWorldExtents(const CTransform3<>& tf3, CVector3<>& rv3_min, CVector3<>& rv3_max) const
	{
		CVector3<> v3_delta	= v3Max * -2;
		CVector3<> v3_x		= tf3.mx3Mat.v3X * v3_delta.tX;
		CVector3<> v3_y		= tf3.mx3Mat.v3Y * v3_delta.tY;
		CVector3<> v3_z		= tf3.mx3Mat.v3Z * v3_delta.tZ;

		// Fill the array.
		CLArray(CVector3<>, pav3, 8);
		pav3[0] = rv3_min = rv3_max = v3Max * tf3;
		pav3[1] = pav3[0] + v3_x;
		pav3[2] = pav3[0] + v3_y;
		pav3[3] = pav3[1] + v3_y;
		pav3[4] = pav3[0] + v3_z;
		pav3[5] = pav3[1] + v3_z;
		pav3[6] = pav3[2] + v3_z;
		pav3[7] = pav3[3] + v3_z;

		for (int i_corner = 1; i_corner < 8; i_corner++)
		{
			rv3_min.SetMin(pav3[i_corner]);
			rv3_max.SetMax(pav3[i_corner]);
		}
	}

	//******************************************************************************************
	CVector3<> CBoundVolBox::v3Clamp(const CVector3<>& v3) const
	{
		CVector3<> v3_ret;
		
		v3_ret.tX = Max(v3.tX, - v3Max.tX);
		v3_ret.tY = Max(v3.tY, - v3Max.tY);
		v3_ret.tZ = Max(v3.tZ, - v3Max.tZ);
		v3_ret.tX = Min(v3_ret.tX, v3Max.tX);
		v3_ret.tY = Min(v3_ret.tY, v3Max.tY);
		v3_ret.tZ = Min(v3_ret.tZ, v3Max.tZ);

		return v3_ret;
	}

	//******************************************************************************************
	CBoundVolBox& CBoundVolBox::operator *=(TReal r_scale)
	{
		// Scale the vector and extent.
		v3Max *= r_scale;
		rExtent *= r_scale;

		return *this;
	}

	//******************************************************************************************
	ESideOf CBoundVolBox::esfSideOf(const CVector3<>& v3) const
	{
		if 
		(
			Abs(v3.tX) <= v3Max.tX &&
			Abs(v3.tY) <= v3Max.tY &&
			Abs(v3.tZ) <= v3Max.tZ
		)
			return esfINSIDE;
		else
			return esfOUTSIDE;
	}

	//******************************************************************************************
	ESideOf CBoundVolBox::esfSideOf(CPArray< CVector3<> > pav3_points) const
	{
		ESideOf esf = 0;
		ESideOf esf_sub;

		esf_sub = CPlaneT<CPlaneDefPosX>(v3Max.tX).esfSideOf(pav3_points);
		if (esf_sub == esfOUTSIDE)
			return esfOUTSIDE;
		esf |= esf_sub;

		esf_sub = CPlaneT<CPlaneDefNegX>(-v3Max.tX).esfSideOf(pav3_points);
		if (esf_sub == esfOUTSIDE)
			return esfOUTSIDE;
		esf |= esf_sub;

		esf_sub = CPlaneT<CPlaneDefPosY>(v3Max.tY).esfSideOf(pav3_points);
		if (esf_sub == esfOUTSIDE)
			return esfOUTSIDE;
		esf |= esf_sub;

		esf_sub = CPlaneT<CPlaneDefNegY>(-v3Max.tY).esfSideOf(pav3_points);
		if (esf_sub == esfOUTSIDE)
			return esfOUTSIDE;
		esf |= esf_sub;

		esf_sub = CPlaneT<CPlaneDefPosZ>(v3Max.tZ).esfSideOf(pav3_points);
		if (esf_sub == esfOUTSIDE)
			return esfOUTSIDE;
		esf |= esf_sub;

		esf_sub = CPlaneT<CPlaneDefNegZ>(-v3Max.tZ).esfSideOf(pav3_points);
		if (esf_sub == esfOUTSIDE)
			return esfOUTSIDE;
		esf |= esf_sub;

		return esf;
	}

	//******************************************************************************************
	ESideOf CBoundVolBox::esfSideOf(const CBoundVolPlane& bvpl, 
		const CPresence3<>* ppr3_this, const CPresence3<>* ppr3_it) const
	{
		// If the volume is esfOUTSIDE, so is the plane.  Otherwise, they intersect.
		return bvpl.esfSideOf(*this, ppr3_it, ppr3_this) == esfOUTSIDE ? esfOUTSIDE : esfINTERSECT;
	}

	//******************************************************************************************
	ESideOf CBoundVolBox::esfSideOf(const CBoundVolSphere& bvs, 
		const CPresence3<>* ppr3_this, const CPresence3<>* ppr3_it) const
	{
		// Intersect with each plane of the box.
		CVolSphere vs_it(bvs.rRadius, ppr3_it);
		if (ppr3_this)
			vs_it /= *ppr3_this;

		if (Abs(vs_it.v3Pos.tX) >= v3Max.tX + vs_it.rRadius ||
			Abs(vs_it.v3Pos.tY) >= v3Max.tY + vs_it.rRadius ||
			Abs(vs_it.v3Pos.tZ) >= v3Max.tZ + vs_it.rRadius)
			return esfOUTSIDE;

		if (Abs(vs_it.v3Pos.tX) < v3Max.tX - vs_it.rRadius &&
			Abs(vs_it.v3Pos.tY) < v3Max.tY - vs_it.rRadius &&
			Abs(vs_it.v3Pos.tZ) < v3Max.tZ - vs_it.rRadius)
			return esfINSIDE;

		// Note: If the sphere intersects planes, it still may be just outside a corner of the box.
		// We currently do not check for this.
		return esfINTERSECT;
	}
	
	//******************************************************************************************
	ESideOf CBoundVolBox::esfSideOf(const CTransform3<>& tf3_box) const
	{
		// Intersect with each plane in turn.
		ESideOf esf = 0;
		ESideOf esf_sub;

		esf_sub = CPlaneT<CPlaneDefPosX>(v3Max.tX).esfSideOf(tf3_box);
		if (esf_sub == esfOUTSIDE)
			return esfOUTSIDE;
		esf |= esf_sub;

		esf_sub = CPlaneT<CPlaneDefNegX>(-v3Max.tX).esfSideOf(tf3_box);
		if (esf_sub == esfOUTSIDE)
			return esfOUTSIDE;
		esf |= esf_sub;

		esf_sub = CPlaneT<CPlaneDefPosY>(v3Max.tY).esfSideOf(tf3_box);
		if (esf_sub == esfOUTSIDE)
			return esfOUTSIDE;
		esf |= esf_sub;

		esf_sub = CPlaneT<CPlaneDefNegY>(-v3Max.tY).esfSideOf(tf3_box);
		if (esf_sub == esfOUTSIDE)
			return esfOUTSIDE;
		esf |= esf_sub;

		esf_sub = CPlaneT<CPlaneDefPosZ>(v3Max.tZ).esfSideOf(tf3_box);
		if (esf_sub == esfOUTSIDE)
			return esfOUTSIDE;
		esf |= esf_sub;

		esf_sub = CPlaneT<CPlaneDefNegZ>(-v3Max.tZ).esfSideOf(tf3_box);
		if (esf_sub == esfOUTSIDE)
			return esfOUTSIDE;
		esf |= esf_sub;

		return esf;
	}

	//******************************************************************************************
	ESideOf CBoundVolBox::esfSideOf(const CBoundVolBox& bvb, 
		const CPresence3<>* ppr3_this, const CPresence3<>* ppr3_it) const
	{
	/*
		// Perform sphere-sphere test first.
		CVolSphere vs_this(rExtent, ppr3_this);
		CVolSphere vs_it(bvb.rExtent, ppr3_it);

		TReal r_sqr_dist = (vs_this.v3Pos - vs_it.v3Pos).tLenSqr();

		TReal r_sum_radius =  vs_this.rRadius + vs_it.rRadius;
		if (r_sqr_dist > r_sum_radius * r_sum_radius)
			return esfOUTSIDE;
	*/

		// Test for aligned boxes.
		if ((!ppr3_this || !ppr3_this->r3Rot.bHasRotation()) &&
			(!ppr3_it || !ppr3_it->r3Rot.bHasRotation()))
		{
			// This should be fast.
			CVector3<> v3_min_this = -v3Max,
					   v3_max_this =  v3Max;
			if (ppr3_this)
			{
				if (ppr3_this->rScale != 1.0)
				{
					v3_min_this *= ppr3_this->rScale;
					v3_max_this *= ppr3_this->rScale;
				}
				v3_min_this += ppr3_this->v3Pos;
				v3_max_this += ppr3_this->v3Pos;
			}

			CVector3<> v3_min_it = -bvb.v3Max,
					   v3_max_it =  bvb.v3Max;
			if (ppr3_it)
			{
				if (ppr3_it->rScale != 1.0)
				{
					v3_min_it *= ppr3_it->rScale;
					v3_max_it *= ppr3_it->rScale;
				}
				v3_min_it += ppr3_it->v3Pos;
				v3_max_it += ppr3_it->v3Pos;
			}

			// Now intersect them.
			if (v3_min_it.tX > v3_max_this.tX || v3_max_it.tX < v3_min_this.tX ||
				v3_min_it.tY > v3_max_this.tY || v3_max_it.tY < v3_min_this.tY ||
				v3_min_it.tZ > v3_max_this.tZ || v3_max_it.tZ < v3_min_this.tZ)
				return esfOUTSIDE;

			if (v3_min_it.tX >= v3_min_this.tX && v3_max_it.tX <= v3_max_this.tX &&
				v3_min_it.tY >= v3_min_this.tY && v3_max_it.tY <= v3_max_this.tY &&
				v3_min_it.tZ >= v3_min_this.tZ && v3_max_it.tZ <= v3_max_this.tZ)
				return esfINSIDE;

			return esfINTERSECT;
		}

		// Perform box-sphere test first.
		CBoundVolSphere bvs_it(bvb.rExtent);
		ESideOf esf = esfSideOf(bvs_it, ppr3_this, ppr3_it);
		if (esf != esfINTERSECT)
			return esf;

		if (ppr3_this || ppr3_it)
		{
			// Get the box transform, transform by the required presences.
			CTransform3<> tf3_box = bvb.tf3Box(pr3Total(ppr3_this, ppr3_it));
			esf = esfSideOf(tf3_box);
		}
		else
		{
			CTransform3<> tf3_box = bvb.tf3Box();
			esf = esfSideOf(tf3_box);
		}

		if (esf == esfINTERSECT)
		{
			// Try to quickly determine whether box really intersects.
			CBoundVolSphere bvs_this(rExtent);
			if (bvb.esfSideOf(bvs_this, ppr3_it, ppr3_this) == esfOUTSIDE)
				return esfOUTSIDE;
		}

		return esf;
	}

	//******************************************************************************************
	ESideOf CBoundVolBox::esfSideOf(const CBoundVolPolyhedron& bvp, 
		const CPresence3<>* ppr3_this, const CPresence3<>* ppr3_it) const
	{
		return bvp.esfIsSideOfPoints(*this, ppr3_it, ppr3_this);
	}

	//******************************************************************************************
	ESideOf CBoundVolBox::esfSideOf(const CBoundVolCamera& bvcam, 
		const CPresence3<>* ppr3_this, const CPresence3<>* ppr3_it) const
	{
		return esfINTERSECT;
	}

	//******************************************************************************************
	CTransform3<> CBoundVolBox::tf3Box() const
	{
	#ifdef __MWERKS__
		return CTransform3<>(CMatrix3<>(2*v3Max.tX,0,0,0,2*v3Max.tY,0,0,0,2*v3Max.tZ), -v3Max);
	#else
		// Create transform describing this box.
		that->mx3Mat.v3X.tX = v3Max.tX * 2;		that->mx3Mat.v3X.tY = that->mx3Mat.v3X.tZ = 0;
		that->mx3Mat.v3Y.tY = v3Max.tY * 2;		that->mx3Mat.v3Y.tX = that->mx3Mat.v3Y.tZ = 0;
		that->mx3Mat.v3Z.tZ = v3Max.tZ * 2;		that->mx3Mat.v3Z.tX = that->mx3Mat.v3Z.tY = 0;
		that->v3Pos = -v3Max;

		return *that;
	#endif
	}

	//******************************************************************************************
	CTransform3<> CBoundVolBox::tf3Box(const CTransform3<>& tf3) const
	{
		// Return the box transform concatenated with the supplied transform.
		// This version optimises for the simplified base box transform.
	#ifdef __MWERKS__
		CTransform3<> rv;		// Return Value
		rv.mx3Mat.v3X = tf3.mx3Mat.v3X * (v3Max.tX * 2);
		rv.mx3Mat.v3Y = tf3.mx3Mat.v3Y * (v3Max.tY * 2);
		rv.mx3Mat.v3Z = tf3.mx3Mat.v3Z * (v3Max.tZ * 2);
		rv.v3Pos = -v3Max * tf3;
		return rv;
	#else
		Assert(that != &tf3);

		that->mx3Mat.v3X = tf3.mx3Mat.v3X * (v3Max.tX * 2);
		that->mx3Mat.v3Y = tf3.mx3Mat.v3Y * (v3Max.tY * 2);
		that->mx3Mat.v3Z = tf3.mx3Mat.v3Z * (v3Max.tZ * 2);
		that->v3Pos = -v3Max * tf3;
		return *that;
	#endif
	}

	//******************************************************************************************
	TReal CBoundVolBox::rGetVolume(TReal r_scale) const
	{
		// Return the volume in world space coordinates.
		return v3Max.tX * v3Max.tY * v3Max.tZ * 8.0 * r_scale * r_scale * r_scale;
	}
	
	//******************************************************************************************
	void CBoundVolBox::Save(int i_handle) const
	{
		int i_num_written;
		
		// Call the base class save function.
		CBoundVol::Save(i_handle);

		// Store the type of bounding volume represented.
		i_num_written = _write(i_handle, (char*)&v3Max, (int)sizeof(v3Max));
		AlwaysAssert(i_num_written == sizeof(v3Max));
		i_num_written = _write(i_handle, (char*)&rExtent, (int)sizeof(rExtent));
		AlwaysAssert(i_num_written == sizeof(rExtent));
	}


//**********************************************************************************************
//
struct SCollisionHelp
//
{
	CBoundVolBox	bvbBox;
	CVector3<>		v3From, v3D;			// Starting and extent vectors.

	// Constructor.
	SCollisionHelp(const CVector3<>& v3_box, const CVector3<>& v3_from, const CVector3<>& v3_d)
		: bvbBox(v3_box), v3From(v3_from), v3D(v3_d)
	{
	}

	//******************************************************************************************
	void Inside(SVolumeLoc* pvl, TReal r_dist, const CDir3<>& d3_face)
	{
		if (bWithin(r_dist, 0, 1))
		{
			if (r_dist < pvl->rDist)
			{
				CVector3<> v3_collision = v3From + v3D * r_dist;

				// A candidate for collision.
				if (bvbBox.esfSideOf(v3_collision) == esfINSIDE)
				{
					// Found it.
					pvl->rDist = r_dist;
					pvl->v3Location = v3_collision;
					pvl->d3Face = d3_face;
				}
			}
		}
	}
};

	//******************************************************************************************
	void TestDist(SVolumeLoc* pvl_result, const CVector3<>& v3_max,
		const CVector3<>& v3_from, const CDir3<>& d3_from, 
		TReal r_diameter, EAxis ea)
	{
		// Create indices describing the other two axes.
		EAxis ea_1 = eaMod(ea+1),
			  ea_2 = eaMod(ea+2);

		// Get the dimensions in these two axes, adjusted inward by the diameter.
		TReal r_max1 = Max(v3_max[ea_1] - r_diameter, 0),
			  r_max2 = Max(v3_max[ea_2] - r_diameter, 0);

		TReal r_dist_sqr = 
			v3_from[ea_1] < -r_max1 ? Sqr(-r_max1 - v3_from[ea_1]) :
			v3_from[ea_1] >  r_max1 ? Sqr( r_max1 - v3_from[ea_1])  
			: 0;
		r_dist_sqr += 
			v3_from[ea_2] < -r_max2 ? Sqr(-r_max2 - v3_from[ea_2]) :
			v3_from[ea_2] >  r_max2 ? Sqr( r_max2 - v3_from[ea_2])
			: 0;

		if (v3_from[ea] > 0.0f)
		{
			// + face.
			// Find distance to nearest point.
			TReal r_dist = fSqrtEst(Sqr(v3_from[ea] - v3_max[ea]) + r_dist_sqr);

			// Add to this a term representing the divergence of the face normal from the 
			// raycast Z.
			r_dist += r_diameter * (1.0f - d3_from[ea]);

			if (r_dist < pvl_result->rDist)
			{
				pvl_result->rDist = r_dist;
				pvl_result->d3Face = ad3Axes[ea];
			}
		}

		if (v3_from[ea] < 0.0f)
		{
			// - face.
			// Find distance to nearest point.
			TReal r_dist = fSqrtEst(Sqr(v3_from[ea] + v3_max[ea]) + r_dist_sqr);

			// Add to this a term representing the divergence of the face normal from the 
			// raycast Z.
			r_dist += r_diameter * (1.0f + d3_from[ea]);

			if (r_dist < pvl_result->rDist)
			{
				pvl_result->rDist = r_dist;
				pvl_result->d3Face = -ad3Axes[ea];
			}
		}
	}

	//**********************************************************************************************
	bool CBoundVolBox::bRayIntersect(SVolumeLoc* pvl_result, const CPresence3<>& pr3_this,
		const CPlacement3<>& p3_origin, TReal r_length, TReal r_diameter) const
	{
		if (r_length <= 0.0)
		{
			// Special flag to find nearest point on volume.
			CVector3<> v3_from = p3_origin.v3Pos / pr3_this;

			//
			// For each face, find the nearest distance to the face, and the angle
			// between the face and the raycast Z, and combine them into a distance value.
			//
			// Find ray's Z direction in local object space.
			CDir3<> d3_z_obj = d3ZAxis * (p3_origin.r3Rot / pr3_this.r3Rot);

			pvl_result->v3Location = v3_from;
			pvl_result->rDist = FLT_MAX;

			r_diameter /= pr3_this.rScale;

			TestDist(pvl_result, v3Max, v3_from, d3_z_obj, r_diameter, eX);
			TestDist(pvl_result, v3Max, v3_from, d3_z_obj, r_diameter, eY);
			TestDist(pvl_result, v3Max, v3_from, d3_z_obj, r_diameter, eZ);

			// Clamp the position onto the chosen box face.
			for (EAxis ea = eX; ea <= eZ; ea = EAxis(ea+1))
			{
				if (pvl_result->d3Face[ea])
					// Set position to lie on the face.
					pvl_result->v3Location[ea] = v3Max[ea] * pvl_result->d3Face[ea];
				else
				{
					// Move inward by diameter.
					if (v3Max[ea] < 2*r_diameter)
						// Side is too small, move position to centre.
						pvl_result->v3Location[ea] = 0;
					else
						SetMinMax(pvl_result->v3Location[ea], 
							-v3Max[ea] + r_diameter, v3Max[ea] - r_diameter);
				}
			}

			pvl_result->iSubObject = 0;
			pvl_result->v3Location *= pr3_this;
			pvl_result->d3Face *= pr3_this.r3Rot;
			pvl_result->rDist  *= pr3_this.rScale;

			return true;
		}

		// Convert origin and endpoint to box space.
		CVector3<> v3_from	= p3_origin.v3Pos / pr3_this;
		CVector3<> v3_d		= CVector3<>(0, r_length / pr3_this.rScale, 0) * 
							  (p3_origin.r3Rot / pr3_this.r3Rot);

		// Set up our helper struct.
		// Store our own dimension, expanded by r_diameter/2.
		CVector3<> v3_box = v3Max * (1 + r_diameter * 0.5 / pr3_this.rScale);
		SCollisionHelp cp(v3_box, v3_from, v3_d);

		pvl_result->rDist = FLT_MAX;

		// Test the six walls of the box.
		if (v3_d.tX != 0.0f)
		{
			cp.Inside(pvl_result, (-v3Max.tX - v3_from.tX) / v3_d.tX, -d3XAxis);
			cp.Inside(pvl_result, ( v3Max.tX - v3_from.tX) / v3_d.tX,  d3XAxis);
		}
		if (v3_d.tY != 0.0f)
		{
			cp.Inside(pvl_result, (-v3Max.tY - v3_from.tY) / v3_d.tY, -d3YAxis);
			cp.Inside(pvl_result, ( v3Max.tY - v3_from.tY) / v3_d.tY,  d3YAxis);
		}
		if (v3_d.tZ != 0.0f)
		{
			cp.Inside(pvl_result, (-v3Max.tZ - v3_from.tZ) / v3_d.tZ, -d3ZAxis);
			cp.Inside(pvl_result, ( v3Max.tZ - v3_from.tZ) / v3_d.tZ,  d3ZAxis);
		}

		if (pvl_result->rDist < FLT_MAX)
		{
			// Found a collision. Convert values to world space.
			pvl_result->v3Location = v3Clamp(pvl_result->v3Location);
			pvl_result->iSubObject = 0;
			pvl_result->rDist = (pvl_result->v3Location - v3_from).tLen() * pr3_this.rScale;
			pvl_result->v3Location *= pr3_this;
			pvl_result->d3Face *= pr3_this.r3Rot;
			return true;
		}
		return false;
	}


