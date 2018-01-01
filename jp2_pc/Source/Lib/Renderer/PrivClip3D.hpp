/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Classes for calculating the point of intersection between an edge of a polygon and a
 *		bounding plane of the view volume.
 *
 *		Note: this is a private header file to the 3d clipping module! It should not be directly
 *		included by any other modules!
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/PrivClip3D.hpp                                           $
 * 
 * 8     97/01/29 18:35 Speter
 * Added #includes due to reorganised ClipDef.hpp.
 * 
 * 7     96/10/30 3:45p Mlange
 * Updated for changes to outcode enum type.
 * 
 * 6     96/09/25 19:50 Speter
 * The Big Change.
 * In transforms, replaced TReal with TR, TDefReal with TReal.
 * Replaced all references to transform templates that have <TObjReal> with <>.
 * Replaced TObjReal with TReal, and "or" prefix with "r".
 * Replaced CObjPoint, CObjNormal, and CObjPlacement with equivalent transform types, and
 * prefixes likewise.
 * Removed some unnecessary casts to TReal.
 * Finally, replaced VER_GENERAL_DEBUG_ASSERTS with VER_DEBUG.
 * 
 * 5     7/25/96 4:29p Mlange
 * The orEdgeT function now takes constant references as parameters instead of pointers. Updated
 * for CObjPoint name change.
 * 
 * 4     96/07/23 11:01 Speter
 * Changed functions to take CPoint parameters rather than CPointClip.
 * 
 * 3     7/18/96 6:59p Mlange
 * Updated for general name change of CVertex to CPoint.
 * 
 * 2     7/16/96 4:55p Mlange
 * The clipping operations have been modified such that the actual plane that is clipped against
 * is moved inwards by half the clip plane thickness value.
 * 
 * 1     7/08/96 5:24p Mlange
 * View volume bounding plane classes.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_RENDERER_PRIVCLIP3D_HPP
#define HEADER_LIB_RENDERER_PRIVCLIP3D_HPP

#include "Lib/Renderer/Clip3D.hpp"
#include <float.h>

//**********************************************************************************************
//
class CViewVolumePlane
//
// Abstract base class that defines the interface for the derived view volume bounding plane
// classes.
//
// Prefix: vvp
//
// Notes:
//		The view volume bounding plane classes contain member functions for calculating the
//		point of intersection between an edge of a polygon and one of the bounding planes of
//		the view volume.
//
//		Note that the bounding planes describe the canonical view volume.
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
	virtual TReal rEdgeT
	(
		const CVector3<>& opt_curr,	// Starting point of the edge, refered to as P0.
		const CVector3<>& opt_next	// Ending point of the edge, refered to as P1.
	) const = 0;
	//
	// Calculate the point of intersection between an edge and this bounding plane of the view
	// volume.
	//
	// Returns:
	//		The parameter of the point of intersection between the edge and the bounding
	//		volume. (e.g this value will be 0 if the point of intersection falls at the
	//		starting point of the edge and 1 if it falls at the end point.)
	//
	// Notes:
	//		The (unavoidable) inaccuracies in the calculation of the intersection point could
	//		result in a point that lies outside of the view volume. To prevent this, this
	//		function moves the bounding plane slightly inwards the view volume by
	//		fFUZZY_ON_CLIP_PLANE / 2 before the intersection point is calculated.
	//
	//		The parameter of the intersection, t, is calculated with the formula:
	//
	//						    N * (P0 - Pe)
	//						t = -------------
	//						       -N * D
	//
	//		Where: Pe is a point on the bounding plane, N is the outward normal of the bounding
	//		plane and D = P1 = P0.
	//
	//		The derivation of this formula can be found in the book: Computer Graphics,
	//		Principles and Practice, Foley et al, Second edition, pages 117 - 119.
	//
	//**************************************


	//******************************************************************************************
	//
	virtual CSet<EOutCodes3D> seteoc3Mask
	(
	) const = 0;
	//
	// Get the set mask for this bounding plane.
	//
	// Returns:
	//		The CSet mask for this bounding plane. This can be used in a set union operation
	//		with the outcode of a point to determine the in/out relationship of that point
	//		with respect to this bounding plane.
	//
	//**************************************
};



//**********************************************************************************************
//
class CViewVolumePlaneLeft : public CViewVolumePlane
//
// Definition of the 'left' view volume bounding plane.
//
// Prefix: vvpl
//
//**************************************
{
public:
	//******************************************************************************************
	//
	// Overrides.
	//

	//******************************************************************************************
	virtual TReal rEdgeT(const CVector3<>& opt_curr, const CVector3<>& opt_next) const
	{
		//
		// For this bounding plane the equation becomes:
		//
		//	    N           Pe               P0 - Pe                     t                =
		//
		//                                                      -(X0 - X) * (Y0 + X)   -X0 - Y0
		//	(-1, -1, 0)  (X, -X, Z)  (X0 - X, Y0 + X, Z0 - Z)   -------------------    --------
		//                                                          -(-dX - dY)         dX + dY
		//
		TReal r_x0 = opt_curr.tX;
		TReal r_x1 = opt_next.tX;
		TReal r_y0 = opt_curr.tY - fFUZZY_ON_CLIP_PLANE / 2;
		TReal r_y1 = opt_next.tY - fFUZZY_ON_CLIP_PLANE / 2;

		return (-r_x0 - r_y0) / ((r_x1 - r_x0) + (r_y1 - r_y0));
	}

	//******************************************************************************************
	virtual CSet<EOutCodes3D> seteoc3Mask() const
	{
		return Set(eoc3OUT_ON_LEFT) + eoc3OUT_LEFT;
	}
};



//**********************************************************************************************
//
class CViewVolumePlaneRight : public CViewVolumePlane
//
// Definition of the 'right' view volume bounding plane.
//
// Prefix: vvpr
//
//**************************************
{
public:
	//******************************************************************************************
	//
	// Overrides.
	//

	//******************************************************************************************
	virtual TReal rEdgeT(const CVector3<>& opt_curr, const CVector3<>& opt_next) const
	{
		//
		// For this bounding plane the equation becomes:
		//
		//	    N           Pe               P0 - Pe                     t                =
		//
		//                                                      (X0 - X) * (Y0 - X)    X0 - Y0
		//	(1, -1, 0)   (X, X, Z)   (X0 - X, Y0 - X, Z0 - Z)   -------------------   --------
		//                                                          -(dX - dY)        -dX + dY
		//
		TReal r_x0 = opt_curr.tX;
		TReal r_x1 = opt_next.tX;
		TReal r_y0 = opt_curr.tY - fFUZZY_ON_CLIP_PLANE / 2;
		TReal r_y1 = opt_next.tY - fFUZZY_ON_CLIP_PLANE / 2;

		return (r_x0 - r_y0) / (-(r_x1 - r_x0) + (r_y1 - r_y0));
	}

	//******************************************************************************************
	virtual CSet<EOutCodes3D> seteoc3Mask() const
	{
		return Set(eoc3OUT_ON_RIGHT) + eoc3OUT_RIGHT;
	}
};



//**********************************************************************************************
//
class CViewVolumePlaneUp : public CViewVolumePlane
//
// Definition of the 'up' view volume bounding plane.
//
// Prefix: vvpu
//
//**************************************
{
public:
	//******************************************************************************************
	//
	// Overrides.
	//

	//******************************************************************************************
	virtual TReal rEdgeT(const CVector3<>& opt_curr, const CVector3<>& opt_next) const
	{
		//
		// For this bounding plane the equation becomes:
		//
		//	    N           Pe               P0 - Pe                     t                =
		//
		//                                                      -(Y0 - Z) + (Z0 - Z)   -Y0 + Z0
		//	(0, -1, 1)   (X, Z, Z)   (X0 - X, Y0 - Z, Z0 - Z)   --------------------   --------
		//                                                          -(-dY + dZ)         dY - dZ
		//
		TReal r_y0 = opt_curr.tY - fFUZZY_ON_CLIP_PLANE / 2;
		TReal r_y1 = opt_next.tY - fFUZZY_ON_CLIP_PLANE / 2;
		TReal r_z0 = opt_curr.tZ;
		TReal r_z1 = opt_next.tZ;

		return (-r_y0 + r_z0) / ((r_y1 - r_y0) - (r_z1 - r_z0));
	}

	//******************************************************************************************
	virtual CSet<EOutCodes3D> seteoc3Mask() const
	{
		return Set(eoc3OUT_ON_UP) + eoc3OUT_UP;
	}
};



//**********************************************************************************************
//
class CViewVolumePlaneDown : public CViewVolumePlane
//
// Definition of the 'down' view volume bounding plane.
//
// Prefix: vvpd
//
//**************************************
{
public:
	//******************************************************************************************
	//
	// Overrides.
	//

	//******************************************************************************************
	virtual TReal rEdgeT(const CVector3<>& opt_curr, const CVector3<>& opt_next) const
	{
		//
		// For this bounding plane the equation becomes:
		//
		//	    N           Pe               P0 - Pe                     t                =
		//
		//                                                      -(Y0 + Z) - (Z0 - Z)   -Y0 - Z0
		//	(0, -1, -1)  (X, -Z, Z)  (X0 - X, Y0 + Z, Z0 - Z)   --------------------   --------
		//                                                          -(-dY - dZ)         dY + dZ
		//
		TReal r_y0 = opt_curr.tY - fFUZZY_ON_CLIP_PLANE / 2;
		TReal r_y1 = opt_next.tY - fFUZZY_ON_CLIP_PLANE / 2;
		TReal r_z0 = opt_curr.tZ;
		TReal r_z1 = opt_next.tZ;

		return (-r_y0 - r_z0) / ((r_y1 - r_y0) + (r_z1 - r_z0));
	}

	//******************************************************************************************
	virtual CSet<EOutCodes3D> seteoc3Mask() const
	{
		return Set(eoc3OUT_ON_DOWN) + eoc3OUT_DOWN;
	}
};



//**********************************************************************************************
//
class CViewVolumePlaneNear : public CViewVolumePlane
//
// Definition of the 'near' view volume bounding plane.
//
// Prefix: vvpn
//
// Notes:
//		This class requires an additional constructor to initialise the data member with the
//		distance from the origin to the near clipping plane.
//
//**************************************
{
	TReal rNearClipDist;

public:
	//******************************************************************************************
	//
	// Constructor.
	//
	CViewVolumePlaneNear(TReal r_near_clip_dist)
	{
		// Adjust clipping plane inwards.
		rNearClipDist = r_near_clip_dist + fFUZZY_ON_CLIP_PLANE / 2;
	}


	//******************************************************************************************
	//
	// Overrides.
	//

	//******************************************************************************************
	virtual TReal rEdgeT(const CVector3<>& opt_curr, const CVector3<>& opt_next) const
	{
		//
		// For this bounding plane the equation becomes:
		//
		//	    N           Pe               P0 - Pe                 t           =
		//
		//                                                      -(Y0 - Yn)   -Y0 + Yn
		//	(0, -1, 0)   (X, Yn, Z)  (X0 - X, Y0 - Yn, Z0 - Z)  ----------   --------
		//                                                        -(-dY)         dY
		//
		TReal r_numer = -opt_curr.tY + rNearClipDist;
		TReal r_denom = opt_next.tY - opt_curr.tY;

		return r_numer / r_denom;
	}

	//******************************************************************************************
	virtual CSet<EOutCodes3D> seteoc3Mask() const
	{
		return Set(eoc3OUT_ON_NEAR) + eoc3OUT_NEAR;
	}
};



//**********************************************************************************************
//
class CViewVolumePlaneFar : public CViewVolumePlane
//
// Definition of the 'far' view volume bounding plane.
//
// Prefix: vvpf
//
// Notes:
//		This class requires an additional constructor to initialise the data member with the
//		distance from the origin to the far clipping plane.
//
//**************************************
{
	TReal rFarClipDist;

public:
	//******************************************************************************************
	//
	// Constructor.
	//
	CViewVolumePlaneFar(TReal r_far_clip_dist)
	{
		// Adjust clipping plane inwards.
		rFarClipDist = r_far_clip_dist - fFUZZY_ON_CLIP_PLANE / 2;
	}


	//******************************************************************************************
	//
	// Overrides.
	//

	//******************************************************************************************
	virtual TReal rEdgeT(const CVector3<>& opt_curr, const CVector3<>& opt_next) const
	{
		//
		// For this bounding plane the equation becomes:
		//
		//	    N           Pe               P0 - Pe                 t           =
		//
		//                                                       (Y0 - Yf)    Y0 - Yf
		//	(0, 1, 0)   (X, Yf, Z)   (X0 - X, Y0 - Yf, Z0 - Z)   ----------   --------
		//                                                          -dY         -dY
		//
		TReal r_numer = opt_curr.tY - rFarClipDist;
		TReal r_denom = -(opt_next.tY - opt_curr.tY);

		return r_numer / r_denom;
	}

	//******************************************************************************************
	virtual CSet<EOutCodes3D> seteoc3Mask() const
	{
		return Set(eoc3OUT_ON_FAR) + eoc3OUT_FAR;
	}
};


#endif
