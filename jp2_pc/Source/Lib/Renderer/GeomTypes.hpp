/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Basic geometry classes:
 *
 *		CBoundVol
 *			CBoundVolInfinite
 *			CBoundVolPoint
 *			CBoundVolSphere
 *			CBoundVolPlane
 *			CBoundVolBox
 *			CBoundVolPolyhedron
 *			CBoundVolCamera
 *
 * Bugs:
 *		The algorithm used in the constructor for the bounding sphere is only approximate. It
 *		needs to be replaced with a better algorithm.
 *
 * To do:
 *		Remove the dummy extern declaration when the compiler has been fixed.
 *
 *		Several functions are not yet implemented....
 *
 *		The constructor for the bounding box class must be updated to generate the most
 *		efficient bounding box for a set points relative to the position.
 *
 *		Consider adding a contructor to the CBoundVolSphere class that calculates the sphere
 *		much faster (but may be less optimal). For example, see: Graphics Gems, page 301.
 *
 *		There is a more efficient algorithm for determining intersections between a box and a
 *		plane. This involves finding the 'p' and 'n' vertices of the box using the octant of the
 *		plane's normal. See Graphics Gems IV, page 74-77.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/GeomTypes.hpp                                            $
 * 
 * 61    98/09/30 18:58 Speter
 * Made CBoundVolBox constructors inline. Added one with specified extent.
 * 
 * 60    98.08.13 4:26p Mmouni
 * Changes for VC++ 5.0sp3 compatibility.
 * 
 * 59    98/06/17 16:50 Speter
 * Fixed CBoundVolPoint implementation; removed unneeded includes.
 * 
 * 58    6/10/98 10:50a Sblack
 * 
 * 57    6/03/98 8:13p Pkeet
 * Added the 'uGetBoxNearestCorner' global function.
 * 
 * 56    98/04/17 17:56 Speter
 * Removed bCollide (obsoleted by bRayIntersect).
 * 
 * 55    98/04/16 14:14 Speter
 * Added bRayIntersect() to bounding volumes, implemented in CBoundVolBox. Eventually will
 * replace bCollide().
 * 
 * 54    98/04/11 0:01 Speter
 * Added CVolSphere::operator /=().
 * 
 * 53    3/06/98 9:16p Agrant
 * clamp function to force a point to be within the bounding volume
 * 
 * 52    2/24/98 3:24p Pkeet
 * Added the 'v3GetMax' member function to the box class.
 * 
 * 51    98/02/04 14:45 Speter
 * Added CBoundVolPoint.
 * 
 * 50    97.12.11 7:13p Mmouni
 * Added += for union to class CBoundVolBox.
 * 
 * 49    97/10/23 2:18p Pkeet
 * Removed the 'tf3Box' member function that takes presences and replaced it with a function
 * that takes no parameters and a function that takes a transform.
 * 
 * 48    97/10/15 6:44p Pkeet
 * Changed 'v3Collide' to 'bCollide.'
 * 
 * 47    97/10/15 1:29a Pkeet
 * Added the 'v3Collide' member function.
 * 
 * 46    97/10/10 12:27p Pkeet
 * Maded the 'tf3Box' function accept presence arguments. Added an 'esfSideOf' that worked with
 * a box transform.
 * 
 * 45    97/10/07 18:12 Speter
 * Changed rGetVolume() to take a scale rather than a whole presence.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_RENDERER_GEOMTYPES_HPP
#define HEADER_LIB_RENDERER_GEOMTYPES_HPP

#include "Lib/Transform/Transform.hpp"
#include "Lib/GeomDBase/Plane.hpp"
#include "Lib/Std/Set.hpp"

// Dummy external declaration to get round a compiler bug.
extern CPlacement3<> p3DUMMY;


//**********************************************************************************************
//
// Definitions for CBoundVol.
//

// Forward declarations of all CBoundVol types.
class CBoundVolInfinite;
class CBoundVolPoint;
class CBoundVolSphere;
class CBoundVolPlane;
class CBoundVolBox;
class CBoundVolPolyhedron;
class CBoundVolCamera;

// Classes used in CBoundVolCamera clipping functions.
class CRenderPolygon;
class CPipelineHeap;
class CCamera;

//**********************************************************************************************
//
enum EOutCode
//
// Prefix: eoc
//
// Outcode flags for six-sided camera bounding volume.
//
//**************************************
{
	eocLEFT, eocRIGHT, eocUP, eocDOWN, eocFAR, eocNEAR,

	// The following flags are used by the pipeline to flag vertices.
	eocUNINIT, eocLIT
};

//**********************************************************************************************
//
enum EBVType
//
// Enumeration type for storing and loading bounding volumes.
//
// Prefix: ebv
//
//**************************************
{
	ebvBASE, ebvINFINITE, ebvSPHERE, ebvPLANE, ebvBOX, ebvPOLYHEDRON, ebvCAMERA, ebvPOINT
};

//**********************************************************************************************
//
struct SVolumeLoc
//
// Prefix: vl
//
// Describes a collision location on an volume.
//
//**************************************
{
	CVector3<>		v3Location;			// The point, in world space, hit.
	CDir3<>			d3Face;				// The normal, in world space, of the face that was hit.
	TReal			rDist;				// The distance of the collision from the cast origin.
	int				iSubObject;			// Indicates which subobject was hit, if applicable; else 0.
};

//**********************************************************************************************
//
class CBoundVol
//
// Abstract base class for bounding volumes.
//
// Prefix: bv
//
//**************************************
{
public:

	//******************************************************************************************
	//
	// Destructor
	//

	virtual ~CBoundVol()
	{
	}

	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	virtual void Save
	(
		int i_handle	// File handle to save to.
	) const;
	//
	// Saves the contents of the object to a file.
	//
	//**********************************

	//******************************************************************************************
	//
	virtual EBVType ebvGetType
	(
	) const
	//
	// Returns the type of bounding volume.
	//
	//**********************************
	{
		return ebvBASE;
	}

	//******************************************************************************************
	//
	virtual int iSizeOf
	(
	) const
	//
	// Returns the size of the object.
	//
	//**********************************
	{
		return sizeof(*this);
	}

	//
	// Intersection functions.
	//

	//******************************************************************************************
	//
	bool bContains
	(
		const CVector3<>& v3			// Point to test, in this volume's space.
	) const
	//
	// Returns:
	//		'true' if the point is inside this volume.
	//
	//**************************************
	{
		return esfSideOf(v3) != esfOUTSIDE;
	}

	//******************************************************************************************
	//
	virtual ESideOf esfSideOf
	(
		const CVector3<>& v3			// Point to test, in this volume's space.
	) const = 0;
	//
	// Returns:
	//		Where this point is in relation to the volume.
	//
	//**********************************

	//******************************************************************************************
	//
	virtual ESideOf esfSideOf
	(
		CPArray< CVector3<> > pav3_solid	// Array of points to test.
	) const = 0;
	//
	// Returns:
	//		Where this solid is in relation to the volume.
	//
	//**********************************

	//******************************************************************************************
	//
	virtual ESideOf esfSideOf
	(
		const CBoundVol& bv,				// The bounding volume to test.
		const CPresence3<>* ppr3_this = 0,	// Optional placement to transform this volume with.
		const CPresence3<>* ppr3_it   = 0	// Optional placement to transform the given volume
											// with.
	) const = 0;
	//
	// Returns:
	//		The relation of the given volume with this volume.
	//
	//**************************************

	//******************************************************************************************
	//
	virtual CTransform3<> tf3Box
	(
	) const;
	//
	// Returns:
	//		A transform which converts the unit cube (0..1) to this box (in object space).
	//
	// Useful for plane intersections.
	//
	//**********************************

	//******************************************************************************************
	//
	virtual CTransform3<> tf3Box
	(
		const CTransform3<>& tf3
	) const;
	//
	// Returns:
	//		A transform which converts the unit cube (0..1) to this box (in object space).
	//
	// Useful for plane intersections.
	//
	//**********************************

	//******************************************************************************************
	//
	virtual ESideOf esfSideOf
	(
		const CTransform3<>& tf3_box	// Transform describing a box in 3-space.
										// Specifies the transformation of a unit cube (0..1).
										// The v3Pos field indicates the position of the box origin.
										// The mx3Mat.v3X, etc. fields describe the length/direction
										// of each axis.
	) const;
	//
	// Returns:
	//		The box's relation to the plane.
	//
	//**********************************
	
	//**********************************************************************************************
	//
	virtual bool bRayIntersect
	(
		SVolumeLoc* pvl_result,			// Returned collision info.
		const CPresence3<>& pr3_this,	// World presence of this volume.
		const CPlacement3<>& p3,		// Origin and direction of ray.
		TReal r_length,					// Length of ray.
		TReal r_diameter				// Width of ray.
	) const
	//
	// Returns:
	//		Whether the ray collided with the volume. If so, pvl_result is filled.
	//
	//**********************************
	{
		return false;
	}


	//
	// Specialised functions for particular bounding volumes.
	//

	//******************************************************************************************
	virtual ESideOf esfSideOf(const CBoundVolInfinite& bvi, 
		const CPresence3<>* ppr3_this = 0, const CPresence3<>* ppr3_it = 0) const
	{
		return esfINTERSECT;
	}

	//******************************************************************************************
	virtual ESideOf esfSideOf(const CBoundVolPoint& bvpt, 
		const CPresence3<>* ppr3_this = 0, const CPresence3<>* ppr3_it = 0) const;

	//******************************************************************************************
	virtual ESideOf esfSideOf(const CBoundVolSphere& bvs, 
		const CPresence3<>* ppr3_this = 0, const CPresence3<>* ppr3_it = 0) const = 0;

	//******************************************************************************************
	virtual ESideOf esfSideOf(const CBoundVolPlane& bvpl, 
		const CPresence3<>* ppr3_this = 0, const CPresence3<>* ppr3_it = 0) const = 0;

	//******************************************************************************************
	virtual ESideOf esfSideOf(const CBoundVolBox& bvb, 
		const CPresence3<>* ppr3_this = 0, const CPresence3<>* ppr3_it = 0) const = 0;

	//******************************************************************************************
	virtual ESideOf esfSideOf(const CBoundVolPolyhedron& bvp, 
		const CPresence3<>* ppr3_this = 0, const CPresence3<>* ppr3_it = 0) const = 0;

	//******************************************************************************************
	virtual ESideOf esfSideOf(const CBoundVolCamera& bvcam, 
		const CPresence3<>* ppr3_this = 0, const CPresence3<>* ppr3_it = 0) const = 0;

	//******************************************************************************************
	//
	virtual ESideOf esfSideOfHelp
	(
		const CBoundVol& bv, 
		const CPresence3<>* ppr3_this = 0, 
		const CPresence3<>* ppr3_it = 0
	) const = 0;
	//
	// Returns:
	//		The relation of this volume to bv (the opposite of bv.esfSideOf(*this).
	//
	// Used as a helper function to implement double dispatch.
	//
	//**********************************

	//******************************************************************************************
	//
	bool bIntersects
	(
		const CBoundVol& bv,				// The bounding volume to test.
		const CPresence3<>* ppr3_this = 0,	// Optional placement to transform this volume with.
		const CPresence3<>* ppr3_it   = 0	// Optional placement to transform the given volume
											// with.
	) const
	//
	// Returns:
	//		'true' if the volumes intersect (including containment), 'false' otherwise.
	//
	//**************************************
	{
		// Default implementation uses esfSideOf, and returns true if any or all of bv is
		// inside this volume.
		return esfSideOf(bv, ppr3_this, ppr3_it) & esfINSIDE;
	}

	//******************************************************************************************
	//
	bool bContains
	(
		const CBoundVol& bv,				// The bounding volume to test.
		const CPresence3<>* ppr3_this = 0,	// Optional placement to transform this volume with.
		const CPresence3<>* ppr3_it   = 0	// Optional placement to transform the given volume
											// with.
	) const
	//
	// Returns:
	//		'true' if the 'it' volume is entirely contained within the 'this' volume.
	//
	//**************************************
	{
		// Default implementation uses esfSideOf, and returns true if all of bv is inside this volume.
		return esfSideOf(bv, ppr3_this, ppr3_it) == esfINSIDE;
	}

	//******************************************************************************************
	//
	virtual void GetWorldExtents
	(
		const CTransform3<>& tf3,		// Transform for volume.
		CVector3<>&         rv3_min,	// Minimum x, y and z values for volume.
		CVector3<>&         rv3_max		// Maximum x, y and z values for volume.
	) const = 0;
	//
	// Sets rv3_min and rv3_max with the minimum and maximum x, y and z values for the bounding
	// volume in the world.
	//
	//**************************************

	//******************************************************************************************
	//
	virtual float fMaxExtent
	(
	) const = 0;
	//
	// Returns the maximum extent (or the distance from the volume's centre to farthest point
	// in the volume, e.g., the radius) of the volume.
	//
	//**************************************

	//******************************************************************************************
	//
	virtual CBoundVolSphere bvsGetBoundingSphere
	(
	) const;
	//
	// Returns a bounding sphere for the bounding volume.
	//
	//**************************************

	//******************************************************************************************
	//
	virtual TReal rGetVolume
	(
		TReal r_scale						// Scale of the instance using this volume.
	) const;
	//
	// Returns the volume of the volume as transformed by the given scale.
	//
	//**************************************

	//
	// Cast functions.
	//

	virtual const CBoundVolInfinite*	pbviCast() const	{ return 0; }
	virtual const CBoundVolPoint*		pbvptCast() const	{ return 0; }
	virtual const CBoundVolSphere*		pbvsCast() const	{ return 0; }
	virtual const CBoundVolPlane*		pbvplCast() const	{ return 0; }
	virtual const CBoundVolBox*			pbvbCast() const	{ return 0; }
	virtual const CBoundVolPolyhedron*	pbvpCast() const	{ return 0; }
	virtual const CBoundVolCamera*		pbvcamCast() const	{ return 0; }
};


//**********************************************************************************************
//
class CBoundVolInfinite : public CBoundVol
//
// A bounding volume with an infinite extent.
//
// Prefix: bvi
//
//**************************************
{
public:
	//******************************************************************************************
	//
	// Constructors and destructor.
	//

	// Default constructor.
	CBoundVolInfinite()
	{
	}

	//******************************************************************************************
	//
	// Overrides.
	//
	
	//******************************************************************************************
	virtual EBVType ebvGetType() const override
	{
		return ebvINFINITE;
	}

	//******************************************************************************************
	virtual int iSizeOf() const override
	{
		return sizeof(*this);
	}

	//******************************************************************************************
	virtual void GetWorldExtents(const CTransform3<>& tf3, CVector3<>& rv3_min, CVector3<>& rv3_max) const override
	{
		rv3_min = CVector3<>(FLT_MIN, FLT_MIN, FLT_MIN);
		rv3_max = CVector3<>(FLT_MAX, FLT_MAX, FLT_MAX);
	}

	//******************************************************************************************
	virtual float fMaxExtent
	(
	) const override
	{
		return FLT_MAX;
	}

	//******************************************************************************************
	virtual TReal rGetVolume(TReal) const override
	{
		return FLT_MAX;
	}

	//******************************************************************************************
	virtual const CBoundVolInfinite* pbviCast() const override { return this; }

	//
	// Any volume whatsoever is inside the infinite volume.
	//

	//******************************************************************************************
	virtual ESideOf esfSideOf(const CVector3<>& v3) const override
	{
		return esfINSIDE;
	}

	//******************************************************************************************
	virtual ESideOf esfSideOf(CPArray< CVector3<> > pav3_solid) const override
	{
		return esfINSIDE;
	}

	//******************************************************************************************
	virtual ESideOf esfSideOf(const CBoundVol& bv, 
		const CPresence3<>* ppr3_this = 0, const CPresence3<>* ppr3_it = 0) const override
	{
		return esfINSIDE;
	}

	//******************************************************************************************
	virtual ESideOf esfSideOf(const CBoundVolInfinite& bvi, 
		const CPresence3<>* ppr3_this = 0, const CPresence3<>* ppr3_it = 0) const override
	{
		return esfINSIDE;
	}

	//******************************************************************************************
	virtual ESideOf esfSideOf(const CBoundVolSphere& bvs, 
		const CPresence3<>* ppr3_this = 0, const CPresence3<>* ppr3_it = 0) const override
	{
		return esfINSIDE;
	}

	//******************************************************************************************
	virtual ESideOf esfSideOf(const CBoundVolPlane& bvpl, 
		const CPresence3<>* ppr3_this = 0, const CPresence3<>* ppr3_it = 0) const override
	{
		return esfINSIDE;
	}

	//******************************************************************************************
	virtual ESideOf esfSideOf(const CBoundVolBox& bvb, 
		const CPresence3<>* ppr3_this = 0, const CPresence3<>* ppr3_it = 0) const override
	{
		return esfINSIDE;
	}

	//******************************************************************************************
	virtual ESideOf esfSideOf(const CBoundVolPolyhedron& bvp, 
		const CPresence3<>* ppr3_this = 0, const CPresence3<>* ppr3_it = 0) const override
	{
		return esfINSIDE;
	}

	//******************************************************************************************
	virtual ESideOf esfSideOf(const CBoundVolCamera& bvcam, 
		const CPresence3<>* ppr3_this = 0, const CPresence3<>* ppr3_it = 0) const override
	{
		return esfINSIDE;
	}

	//******************************************************************************************
	virtual ESideOf esfSideOfHelp(const CBoundVol& bv, 
		const CPresence3<>* ppr3_this = 0, const CPresence3<>* ppr3_it = 0) const override
	{
		return esfINTERSECT;
	}

	//******************************************************************************************
	//
	// Operators.
	//

	//******************************************************************************************
	CBoundVolInfinite& operator *=(TReal r_scale)
	{
		return *this;
	}

	//******************************************************************************************
	CBoundVolInfinite& operator *=(const CPresence3<>& pr3)
	{
		return *this;
	}
};

//**********************************************************************************************
//
class CBoundVolPoint : public CBoundVol
//
// A bounding volume with no extent.
//
// Prefix: bvpt
//
//**************************************
{
public:
	//******************************************************************************************
	//
	// Constructors and destructor.
	//

	// Default constructor.
	CBoundVolPoint()
	{
	}

	//******************************************************************************************
	//
	// Overrides.
	//
	
	//******************************************************************************************
	virtual EBVType ebvGetType() const override
	{
		return ebvPOINT;
	}

	//******************************************************************************************
	virtual int iSizeOf() const override
	{
		return sizeof(*this);
	}

	//******************************************************************************************
	virtual void GetWorldExtents(const CTransform3<>& tf3, CVector3<>& rv3_min, CVector3<>& rv3_max) const override
	{
		rv3_min = rv3_max = tf3.v3Pos;
	}

	//******************************************************************************************
	virtual float fMaxExtent() const override
	{
		return 0;
	}

	//******************************************************************************************
	virtual TReal rGetVolume(TReal) const override
	{
		return 0;
	}

	//******************************************************************************************
	virtual const CBoundVolPoint* pbvptCast() const override { return this; }

	//******************************************************************************************
	virtual ESideOf esfSideOf(const CVector3<>& v3) const override;

	//******************************************************************************************
	virtual ESideOf esfSideOf(CPArray< CVector3<> > pav3_solid) const override;

	//******************************************************************************************
	virtual ESideOf esfSideOf(const CBoundVol& bv, 
		const CPresence3<>* ppr3_this = 0, const CPresence3<>* ppr3_it = 0) const override;

	//******************************************************************************************
	virtual ESideOf esfSideOf(const CBoundVolInfinite& bvi, 
		const CPresence3<>* ppr3_this = 0, const CPresence3<>* ppr3_it = 0) const override;

	//******************************************************************************************
	virtual ESideOf esfSideOf(const CBoundVolPoint& bvpt, 
		const CPresence3<>* ppr3_this = 0, const CPresence3<>* ppr3_it = 0) const override;

	//******************************************************************************************
	virtual ESideOf esfSideOf(const CBoundVolSphere& bvs, 
		const CPresence3<>* ppr3_this = 0, const CPresence3<>* ppr3_it = 0) const override;

	//******************************************************************************************
	virtual ESideOf esfSideOf(const CBoundVolPlane& bvpl, 
		const CPresence3<>* ppr3_this = 0, const CPresence3<>* ppr3_it = 0) const override;

	//******************************************************************************************
	virtual ESideOf esfSideOf(const CBoundVolBox& bvb, 
		const CPresence3<>* ppr3_this = 0, const CPresence3<>* ppr3_it = 0) const override;

	//******************************************************************************************
	virtual ESideOf esfSideOf(const CBoundVolPolyhedron& bvp, 
		const CPresence3<>* ppr3_this = 0, const CPresence3<>* ppr3_it = 0) const override;

	//******************************************************************************************
	virtual ESideOf esfSideOf(const CBoundVolCamera& bvcam, 
		const CPresence3<>* ppr3_this = 0, const CPresence3<>* ppr3_it = 0) const override;

	//******************************************************************************************
	virtual ESideOf esfSideOfHelp(const CBoundVol& bv, 
		const CPresence3<>* ppr3_this = 0, const CPresence3<>* ppr3_it = 0) const override
	{
		return bv.esfSideOf(*this, ppr3_it, ppr3_this);
	}
};

//**********************************************************************************************
//
class CBoundVolSphere : public CBoundVol
//
// Simple 3d centred bounding sphere.
//
// Prefix: bvs
//
//**************************************
{
public:
	TReal rRadius;					// The radius of the sphere.

	//******************************************************************************************
	//
	// Constructors and destructor.
	//

	// Default constructor.
	CBoundVolSphere()
	{
	}

	// Initialise with radius.
	CBoundVolSphere(TReal r_radius)
		: rRadius(r_radius)
	{
		Assert(r_radius > 0);
	}


	// Initialise with an array of points.
	CBoundVolSphere(CPArray< CVector3<> > pav3);

	// Initialize from a file.
	CBoundVolSphere(int i_handle);


	//******************************************************************************************
	//
	// Overrides.
	//

	//******************************************************************************************
	virtual void Save(int i_handle) const override;
	
	//******************************************************************************************
	virtual EBVType ebvGetType() const override
	{
		return ebvSPHERE;
	}

	//******************************************************************************************
	virtual int iSizeOf() const override
	{
		return sizeof(*this);
	}

	//******************************************************************************************
	virtual void GetWorldExtents(const CTransform3<>& tf3, CVector3<>& rv3_min, CVector3<>& rv3_max) const override;

	//******************************************************************************************
	virtual float fMaxExtent
	(
	) const override
	{
		return rRadius;
	}

	virtual const CBoundVolSphere* pbvsCast() const override { return this; }

	//******************************************************************************************
	virtual ESideOf esfSideOf(const CVector3<>& v3) const override;

	//******************************************************************************************
	virtual ESideOf esfSideOf(CPArray< CVector3<> > pav3_solid) const override;

	//******************************************************************************************
	virtual ESideOf esfSideOf(const CBoundVol& bv, 
		const CPresence3<>* ppr3_this = 0, const CPresence3<>* ppr3_it = 0) const override
	{
		// Invoke helper function for double dispatch.
		return bv.esfSideOfHelp(*this, ppr3_it, ppr3_this);
	}

	//******************************************************************************************
	virtual ESideOf esfSideOf(const CBoundVolInfinite& bvi, 
		const CPresence3<>* ppr3_this = 0, const CPresence3<>* ppr3_it = 0) const override
	{
		return esfINTERSECT;
	}

	//******************************************************************************************
	virtual ESideOf esfSideOf(const CBoundVolPoint& bvpt, 
		const CPresence3<>* ppr3_this = 0, const CPresence3<>* ppr3_it = 0) const override
	{
		return CBoundVol::esfSideOf(bvpt, ppr3_this, ppr3_it);
	}

	//******************************************************************************************
	virtual ESideOf esfSideOf(const CBoundVolSphere& bvs, 
		const CPresence3<>* ppr3_this = 0, const CPresence3<>* ppr3_it = 0) const override;

	//******************************************************************************************
	virtual ESideOf esfSideOf(const CBoundVolPlane& bvpl, 
		const CPresence3<>* ppr3_this = 0, const CPresence3<>* ppr3_it = 0) const override;

	//******************************************************************************************
	virtual ESideOf esfSideOf(const CBoundVolBox& bvb, 
		const CPresence3<>* ppr3_this = 0, const CPresence3<>* ppr3_it = 0) const override;

	//******************************************************************************************
	virtual ESideOf esfSideOf(const CBoundVolPolyhedron& bvp, 
		const CPresence3<>* ppr3_this = 0, const CPresence3<>* ppr3_it = 0) const override;

	//******************************************************************************************
	virtual ESideOf esfSideOf(const CBoundVolCamera& bvcam, 
		const CPresence3<>* ppr3_this = 0, const CPresence3<>* ppr3_it = 0) const override;

	//******************************************************************************************
	virtual ESideOf esfSideOfHelp(const CBoundVol& bv, 
		const CPresence3<>* ppr3_this = 0, const CPresence3<>* ppr3_it = 0) const override
	{
		// Implement helper function for double dispatch.
		return bv.esfSideOf(*this, ppr3_it, ppr3_this);
	}

	//******************************************************************************************
	//
	// Operators.
	//

	//******************************************************************************************
	CBoundVolSphere& operator *=(TReal r_scale)
	{
		rRadius *= r_scale;
		return *this;
	}
};

//**********************************************************************************************
//
class CVolSphere
//
// A sphere object with position and radius.
//
// Prefix: vs
//
//**************************************
{
public:
	TReal rRadius;						// The radius of the sphere.
	CVector3<> v3Pos;					// Its position.

	//******************************************************************************************
	CVolSphere(TReal r_radius)
		: rRadius(r_radius), v3Pos(0, 0, 0)
	{
	}

	//******************************************************************************************
	CVolSphere(TReal r_radius, const CVector3<>& v3_pos)
		: rRadius(r_radius), v3Pos(v3_pos)
	{
	}

	//******************************************************************************************
	CVolSphere(TReal r_radius, const CPresence3<>* ppr3)
		: rRadius(r_radius)
	{
		if (ppr3)
		{
			rRadius *= ppr3->rScale;
			v3Pos = ppr3->v3Pos;
		}
		else
			v3Pos = CVector3<>(0, 0, 0);
	}

	//******************************************************************************************
	CVolSphere& operator *=(const CPresence3<>& pr3)
	{
		rRadius *= pr3.rScale;
		v3Pos *= pr3;
		return *this;
	}

	//******************************************************************************************
	CVolSphere& operator /=(const CPresence3<>& pr3)
	{
		rRadius /= pr3.rScale;
		v3Pos /= pr3;
		return *this;
	}

	//******************************************************************************************
	CVolSphere& operator *=(TReal r_scale)
	{
		rRadius *= r_scale;
		return *this;
	}
};

//**********************************************************************************************
//
class CBoundVolPlane : public CBoundVol, public CPlaneTolerance
//
// A bounding volume defined by a plane; the interior of the volume consists of the inside
// (negative side) of the plane.
//
// Prefix: bvpl
//
//**************************************
{
public:

	//******************************************************************************************
	//
	// Constructors.
	//

	// Initialise with a CPlaneTolerance
	CBoundVolPlane(const CPlaneTolerance& plt)
		: CPlaneTolerance(plt)
	{
	}

	// Initialise with a plane.
	CBoundVolPlane(const CPlane& pl, TReal r_tolerance = fPLANE_TOLERANCE)
		: CPlaneTolerance(pl, r_tolerance)
	{
	}

	//******************************************************************************************
	//
	// Overrides.
	//

	//******************************************************************************************
	virtual void Save(int i_handle) const override
	{
		Assert(0);
	}
	
	//******************************************************************************************
	virtual EBVType ebvGetType() const override
	{
		return ebvPLANE;
	}

	//******************************************************************************************
	virtual int iSizeOf() const override
	{
		return sizeof(*this);
	}

	//******************************************************************************************
	virtual void GetWorldExtents(const CTransform3<>& tf3, CVector3<>& rv3_min, CVector3<>& rv3_max) const override
	{
		rv3_min = CVector3<>(FLT_MIN, FLT_MIN, FLT_MIN);
		rv3_max = CVector3<>(FLT_MAX, FLT_MAX, FLT_MAX);
	}
	
	//******************************************************************************************
	virtual ESideOf esfSideOf(const CTransform3<>& tf3_box) const override;

	//******************************************************************************************
	virtual float fMaxExtent() const override
	{
		return FLT_MAX;
	}

	//******************************************************************************************
	virtual ESideOf esfSideOf(const CVector3<>& v3) const override
	{
		return CPlaneTolerance::esfSideOf(v3);
	}

	//******************************************************************************************
	virtual ESideOf esfSideOf(CPArray< CVector3<> > pav3_solid) const override
	{
		return CPlaneTolerance::esfSideOf(pav3_solid);
	}

	//******************************************************************************************
	virtual ESideOf esfSideOf(const CBoundVol& bv, 
		const CPresence3<>* ppr3_this = 0, const CPresence3<>* ppr3_it = 0) const override
	{
		// Invoke helper function for double dispatch.
		return bv.esfSideOfHelp(*this, ppr3_it, ppr3_this);
	}

	//******************************************************************************************
	virtual ESideOf esfSideOf(const CBoundVolInfinite& bvi, 
		const CPresence3<>* ppr3_this = 0, const CPresence3<>* ppr3_it = 0) const override
	{
		return esfINTERSECT;
	}

	//******************************************************************************************
	virtual ESideOf esfSideOf(const CBoundVolPoint& bvpt, 
		const CPresence3<>* ppr3_this = 0, const CPresence3<>* ppr3_it = 0) const override
	{
		return CBoundVol::esfSideOf(bvpt, ppr3_this, ppr3_it);
	}

	//******************************************************************************************
	virtual ESideOf esfSideOf(const CBoundVolSphere& bvs, 
		const CPresence3<>* ppr3_this = 0, const CPresence3<>* ppr3_it = 0) const override;

	//******************************************************************************************
	virtual ESideOf esfSideOf(const CBoundVolPlane& bvpl, 
		const CPresence3<>* ppr3_this = 0, const CPresence3<>* ppr3_it = 0) const override;

	//******************************************************************************************
	virtual ESideOf esfSideOf(const CBoundVolBox& bvb, 
		const CPresence3<>* ppr3_this = 0, const CPresence3<>* ppr3_it = 0) const override;

	//******************************************************************************************
	virtual ESideOf esfSideOf(const CBoundVolPolyhedron& bvp, 
		const CPresence3<>* ppr3_this = 0, const CPresence3<>* ppr3_it = 0) const override;

	//******************************************************************************************
	virtual ESideOf esfSideOf(const CBoundVolCamera& bvcam, 
		const CPresence3<>* ppr3_this = 0, const CPresence3<>* ppr3_it = 0) const override;

	//******************************************************************************************
	virtual ESideOf esfSideOfHelp(const CBoundVol& bv, 
		const CPresence3<>* ppr3_this = 0, const CPresence3<>* ppr3_it = 0) const override
	{
		// Implement helper function for double dispatch.
		return bv.esfSideOf(*this, ppr3_it, ppr3_this);
	}

	//******************************************************************************************
	virtual TReal rGetVolume(TReal) const override
	{
		return FLT_MAX;
	}

/*
	//******************************************************************************************
	//
	// Operators.
	//

	//******************************************************************************************
	CBoundVolPlane& operator *=(TReal r_scale)
	{
		CPlaneTolerance::operator *=(r_scale);
		return *this;
	}

	//******************************************************************************************
	CBoundVol& operator *=(const CPresence3<>& pr3)
	{
		CPlaneTolerance::operator *=(pr3);
		return *this;
	}

	//******************************************************************************************
	CBoundVolPlane operator *(const CPresence3<>& pr3) const
	{
		CBoundVolPlane bvp = *this;
		bvp *= pr3;
		return bvp;
	}
*/

public:
};

//**********************************************************************************************
//
// Definitions for CBoundVolBox.
//

enum EBoxExtents
// Parameter type to obtain a box' extents.
// Prefix: ebe
{
	ebeMIN_X, ebeMAX_X, ebeMIN_Y, ebeMAX_Y, ebeMIN_Z, ebeMAX_Z
};


//**********************************************************************************************
//
class CBoundVolBox : public CBoundVol
//
// Axis aligned, centred, 3d bounding box.
//
// Prefix: bvb
//
//**************************************
{
	CVector3<>	v3Max;					// A corner of the box (all coords positive).
	TReal		rExtent;				// Maximum extent (might be smaller than length of
										// v3Max if constructed from a point cloud).

public:

	//******************************************************************************************
	//
	// Constructors and destructor.
	//

	// Default constructor.
	CBoundVolBox()
	{
	}

	// Initialise with a corner.
	CBoundVolBox(CVector3<> v3_corner)
		: v3Max(v3_corner)
	{
		Assert(v3Max.tX >= 0 && v3Max.tY >= 0 && v3Max.tZ >= 0);

		// Set rExtent to the length of the half-diagonal.
		rExtent = v3Max.tLen();
	}

	// Initialise with a corner, and specified first-pass length.
	// Avoids the length calculation.
	CBoundVolBox(CVector3<> v3_corner, TReal r_extent)
		: v3Max(v3_corner), rExtent(r_extent)
	{
		Assert(v3Max.tX >= 0 && v3Max.tY >= 0 && v3Max.tZ >= 0);
		Assert(rExtent >= 0);
	}


	// Initialise with the given dimensions along each axis (from end to end).
	CBoundVolBox(TReal r_dimx, TReal r_dimy, TReal r_dimz)
		: v3Max(r_dimx * 0.5f, r_dimy * 0.5f, r_dimz * 0.5f)
	{
		Assert(v3Max.tX >= 0 && v3Max.tY >= 0 && v3Max.tZ >= 0);

		rExtent = v3Max.tLen();
	}


	// Initialise with an array of points.
	CBoundVolBox(CPArray< CVector3<> > pav3);

	// Initialize from a file.
	CBoundVolBox(int i_handle);


	//******************************************************************************************
	//
	// Operators.
	//

	// Get a corner of the bounding box.
	CVector3<> operator [](uint u_corner) const;

	// Obtain the extents of the bounding box.
	TReal operator [](EBoxExtents ebe) const;

	// Union of the box with another box.
	CBoundVolBox& operator +=(const CBoundVolBox &bvb)
	{
		v3Max.SetMax(bvb.v3Max);
		rExtent = Max(rExtent, bvb.rExtent);
		return *this;
	}

	//******************************************************************************************
	//
	// Overrides.
	//

	//******************************************************************************************
	virtual void Save(int i_handle) const override;
	
	//******************************************************************************************
	virtual EBVType ebvGetType() const override
	{
		return ebvBOX;
	}

	//******************************************************************************************
	//
	CVector3<> v3GetMax
	(
	) const
	//
	// Return the maximum values for the box.
	//
	//**********************************
	{
		return v3Max;
	}

	//******************************************************************************************
	virtual int iSizeOf() const override
	{
		return sizeof(*this);
	}

	//******************************************************************************************
	virtual void GetWorldExtents(const CTransform3<>& tf3, CVector3<>& rv3_min, CVector3<>& rv3_max) const override;

	//******************************************************************************************
	virtual float fMaxExtent() const override
	{
		return rExtent;
	}


	//******************************************************************************************
	virtual CVector3<> v3Clamp(const CVector3<> &v3) const;
	//
	//  Returns the point in "this" closest to "v3"
	//

	//******************************************************************************************
	virtual const CBoundVolBox* pbvbCast() const override { return this; }

	//
	// Intersection functions.
	//
	
	//******************************************************************************************
	virtual ESideOf esfSideOf(const CTransform3<>& tf3_box) const override;
	
	//******************************************************************************************
	virtual bool bRayIntersect(SVolumeLoc* pvl_result, const CPresence3<>& pr3_this,
		const CPlacement3<>& p3_origin, TReal r_length, TReal r_diameter) const override;

	//******************************************************************************************
	virtual ESideOf esfSideOf(const CVector3<>& v3) const override;

	//******************************************************************************************
	virtual ESideOf esfSideOf(CPArray< CVector3<> > pav3_solid) const override;

	//******************************************************************************************
	virtual ESideOf esfSideOf(const CBoundVol& bv, 
		const CPresence3<>* ppr3_this = 0, const CPresence3<>* ppr3_it = 0) const override
	{
		// Invoke helper function for double dispatch.
		return bv.esfSideOfHelp(*this, ppr3_it, ppr3_this);
	}

	//******************************************************************************************
	virtual ESideOf esfSideOf(const CBoundVolInfinite& bvi, 
		const CPresence3<>* ppr3_this = 0, const CPresence3<>* ppr3_it = 0) const override
	{
		return esfINTERSECT;
	}

	//******************************************************************************************
	virtual ESideOf esfSideOf(const CBoundVolPoint& bvpt, 
		const CPresence3<>* ppr3_this = 0, const CPresence3<>* ppr3_it = 0) const override
	{
		return CBoundVol::esfSideOf(bvpt, ppr3_this, ppr3_it);
	}

	//******************************************************************************************
	virtual ESideOf esfSideOf(const CBoundVolPlane& bvpl, 
		const CPresence3<>* ppr3_this = 0, const CPresence3<>* ppr3_it = 0) const override;

	//******************************************************************************************
	virtual ESideOf esfSideOf(const CBoundVolSphere& bvs, 
		const CPresence3<>* ppr3_this = 0, const CPresence3<>* ppr3_it = 0) const override;

	//******************************************************************************************
	virtual ESideOf esfSideOf(const CBoundVolBox& bvb, 
		const CPresence3<>* ppr3_this = 0, const CPresence3<>* ppr3_it = 0) const override;

	//******************************************************************************************
	virtual ESideOf esfSideOf(const CBoundVolPolyhedron& bvp, 
		const CPresence3<>* ppr3_this = 0, const CPresence3<>* ppr3_it = 0) const override;

	//******************************************************************************************
	virtual ESideOf esfSideOf(const CBoundVolCamera& bvcam, 
		const CPresence3<>* ppr3_this = 0, const CPresence3<>* ppr3_it = 0) const override;

	//******************************************************************************************
	virtual ESideOf esfSideOfHelp(const CBoundVol& bv, 
		const CPresence3<>* ppr3_this = 0, const CPresence3<>* ppr3_it = 0) const override
	{
		// Implement helper function for double dispatch.
		return bv.esfSideOf(*this, ppr3_it, ppr3_this);
	}

	//******************************************************************************************
	//
	virtual CTransform3<> tf3Box
	(
	) const override;
	//
	// Returns:
	//		A transform which converts the unit cube (0..1) to this box (in object space).
	//
	// Useful for plane intersections.
	//
	//**********************************

	//******************************************************************************************
	//
	virtual CTransform3<> tf3Box
	(
		const CTransform3<>& tf3
	) const override;
	//
	// Returns:
	//		A transform which converts the unit cube (0..1) to this box (in object space).
	//
	// Useful for plane intersections.
	//
	//**********************************

	//******************************************************************************************
	virtual TReal rGetVolume(TReal r_scale) const override;

	//******************************************************************************************
	//
	// Operators.
	//

	//******************************************************************************************
	CBoundVolBox& operator *=(TReal r_scale);
};


//**********************************************************************************************
//
typedef CPlaneTol<1000> CPlanePoly;
//
// Prefix: pl
//
// Plane class with specialised tolerance, used for polyhedra.
//
//**************************************

//**********************************************************************************************
//
class CBoundVolPolyhedron : public CBoundVol
//
// Convex bounding polyhedron.
//
// Prefix: bvp
//
//**************************************
{
	CBoundVolSphere bvsSphere;	// The bounding sphere of this polyhedron. It is useful for quick
								// initial intersection tests.

public:

	typedef CPArray< CVector3<> >	TVertexList;
	typedef CPArray<CPlanePoly>		TPlaneList;

	// The polygons that define the convex polyhedron are specified by a set of plane equations and
	// a set of vertices. The vertices correspond to the intersection points between the planes.

	CAArray< CVector3<> > lv3Vertices;	// The set of vertices
	CAArray<CPlanePoly> lplPlanes;		// The set of planes bounding the volume. Note that the normal
										// of each plane points outwards the volume.

	//******************************************************************************************
	//
	// Constructors and destructor.
	//

	// Default constructor.
	CBoundVolPolyhedron()
	{
	}

	virtual ~CBoundVolPolyhedron()
	{
	}

	CBoundVolPolyhedron(CPArray<CPlane> papl, CPArray< CVector3<> > pav3);
	// Initialise with an array of planes and vertices. No checks are made to ensure the validity
	// of the volume!  The arrays must not be temporary, as these same arrays are used for storage.

	CBoundVolPolyhedron(const CBoundVolPolyhedron& bvp);
	// Special copy constructor to duplicate CAArray.

	//******************************************************************************************
	//
	// Overrides.
	//

	//******************************************************************************************
	virtual void Save(int i_handle) const override
	{
		Assert(0);
	}
	
	//******************************************************************************************
	virtual EBVType ebvGetType() const override
	{
		return ebvPOLYHEDRON;
	}

	//******************************************************************************************
	virtual int iSizeOf() const override
	{
		return sizeof(*this);
	}

	//******************************************************************************************
	virtual void GetWorldExtents(const CTransform3<>& tf3, CVector3<>& rv3_min, CVector3<>& rv3_max) const override;

	//******************************************************************************************
	virtual float fMaxExtent
	(
	) const override
	{
		return bvsSphere.rRadius;
	}

	//******************************************************************************************
	virtual ESideOf esfSideOf(const CTransform3<>& tf3_box) const override;

	//******************************************************************************************
	virtual const CBoundVolPolyhedron* pbvpCast() const override { return this; }

	//******************************************************************************************
	virtual ESideOf esfSideOf(const CVector3<>& v3) const override;

	//******************************************************************************************
	virtual ESideOf esfSideOf(CPArray< CVector3<> > pav3_solid) const override;

	//******************************************************************************************
	virtual ESideOf esfSideOf(const CBoundVol& bv, 
		const CPresence3<>* ppr3_this = 0, const CPresence3<>* ppr3_it = 0) const override
	{
		// Invoke helper function for double dispatch.
		return bv.esfSideOfHelp(*this, ppr3_it, ppr3_this);
	}

	//******************************************************************************************
	virtual ESideOf esfSideOf(const CBoundVolInfinite& bvi, 
		const CPresence3<>* ppr3_this = 0, const CPresence3<>* ppr3_it = 0) const override
	{
		return esfINTERSECT;
	}

	//******************************************************************************************
	virtual ESideOf esfSideOf(const CBoundVolPoint& bvpt, 
		const CPresence3<>* ppr3_this = 0, const CPresence3<>* ppr3_it = 0) const override
	{
		return CBoundVol::esfSideOf(bvpt, ppr3_this, ppr3_it);
	}

	//******************************************************************************************
	virtual ESideOf esfSideOf(const CBoundVolSphere& bvs, 
		const CPresence3<>* ppr3_this = 0, const CPresence3<>* ppr3_it = 0) const override;

	//******************************************************************************************
	virtual ESideOf esfSideOf(const CBoundVolPlane& bvpl, 
		const CPresence3<>* ppr3_this = 0, const CPresence3<>* ppr3_it = 0) const override;

	//******************************************************************************************
	virtual ESideOf esfSideOf(const CBoundVolBox& bvb, 
		const CPresence3<>* ppr3_this = 0, const CPresence3<>* ppr3_it = 0) const override;

	//******************************************************************************************
	virtual ESideOf esfSideOf(const CBoundVolPolyhedron& bvp, 
		const CPresence3<>* ppr3_this = 0, const CPresence3<>* ppr3_it = 0) const override;

	//******************************************************************************************
	virtual ESideOf esfSideOf(const CBoundVolCamera& bvcam, 
		const CPresence3<>* ppr3_this = 0, const CPresence3<>* ppr3_it = 0) const override;

	//******************************************************************************************
	virtual ESideOf esfSideOfHelp(const CBoundVol& bv, 
		const CPresence3<>* ppr3_this = 0, const CPresence3<>* ppr3_it = 0) const override
	{
		// Implement helper function for double dispatch.
		return bv.esfSideOf(*this, ppr3_it, ppr3_this);
	}

	//******************************************************************************************
	ESideOf esfIsSideOfPoints(const CBoundVol& bv, 
		const CPresence3<>* ppr3_this = 0, const CPresence3<>* ppr3_it = 0) const;
	//
	// Helper function for other classes intersecting with CBoundVolPolyhedron.
	//		

	//******************************************************************************************
	//
	// Operators.
	//

	//******************************************************************************************
	CBoundVolPolyhedron& operator *=(TReal r_scale);

	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	virtual ESideOf esfClipPolygonInside
	(
		CRenderPolygon& rpoly,			// The render polygon to clip.
		CPipelineHeap& plh				// The heap object to allocate new vertices on.
	) const;
	//
	// Identical in function to CClipPlane::bClipPolygonInside, but always interpolates v3Screen
	// coords linearly.
	//
	//**********************************
};


// Had to create this const outside the default argument list for VC5.0
const CSet<EOutCode> seteoc_esfClipPolygonInside_default_arg = -CSet<EOutCode>();


//**********************************************************************************************
//
class CBoundVolCamera : public CBoundVol
//
// Prefix: bvcam
//
// An abstract class which implements clipping for a six-sided camera volume.
// It is also usable as a spatial bounding volume class, for object intersection tests.
// However, currently it is only useful on the left side of a bvcam.esfSideOf(bv) expression.
// Any usage such as bv.esfSideOf(bvcam) will return esfINTERSECT.  Also, none of the 
// extent access functions such as fMaxExtent() and GetWorldExtents() are usefully
// implemented.
//

//**************************************
{
public:

	//******************************************************************************************
	//
	// Overrides.
	//

	//******************************************************************************************
	virtual void Save(int i_handle) const override
	{
		Assert(0);
	}
	
	//******************************************************************************************
	virtual EBVType ebvGetType() const override
	{
		return ebvCAMERA;
	}

	//******************************************************************************************
	virtual int iSizeOf() const override
	{
		return sizeof(*this);
	}

	//******************************************************************************************
	virtual ESideOf esfSideOf(const CTransform3<>& tf3_box) const override = 0;

	//******************************************************************************************
	virtual ESideOf esfSideOf(const CVector3<>& v3) const override = 0;

	//******************************************************************************************
	virtual ESideOf esfSideOf(CPArray< CVector3<> > pav3_solid) const override = 0;

	//******************************************************************************************
	virtual ESideOf esfSideOf(const CBoundVol& bv, 
		const CPresence3<>* ppr3_this = 0, const CPresence3<>* ppr3_it = 0) const override
	{
		// Invoke helper function for double dispatch.
		return bv.esfSideOfHelp(*this, ppr3_it, ppr3_this);
	}

	//******************************************************************************************
	virtual ESideOf esfSideOf(const CBoundVolInfinite& bvi, 
		const CPresence3<>* ppr3_this = 0, const CPresence3<>* ppr3_it = 0) const override
	{
		return esfINTERSECT;
	}

	//******************************************************************************************
	virtual ESideOf esfSideOf(const CBoundVolPoint& bvpt, 
		const CPresence3<>* ppr3_this = 0, const CPresence3<>* ppr3_it = 0) const override
	{
		return CBoundVol::esfSideOf(bvpt, ppr3_this, ppr3_it);
	}

	//******************************************************************************************
	virtual ESideOf esfSideOf(const CBoundVolSphere& bvs, 
		const CPresence3<>* ppr3_this = 0, const CPresence3<>* ppr3_it = 0) const override = 0;

	//******************************************************************************************
	virtual ESideOf esfSideOf(const CBoundVolPlane& bvpl, 
		const CPresence3<>* ppr3_this = 0, const CPresence3<>* ppr3_it = 0) const override = 0;

	//******************************************************************************************
	virtual ESideOf esfSideOf(const CBoundVolBox& bvb, 
		const CPresence3<>* ppr3_this = 0, const CPresence3<>* ppr3_it = 0) const override = 0;

	//******************************************************************************************
	virtual ESideOf esfSideOf(const CBoundVolPolyhedron& bvp, 
		const CPresence3<>* ppr3_this = 0, const CPresence3<>* ppr3_it = 0) const override = 0;

	//******************************************************************************************
	virtual ESideOf esfSideOf(const CBoundVolCamera& bvcam, 
		const CPresence3<>* ppr3_this = 0, const CPresence3<>* ppr3_it = 0) const override = 0;

	//******************************************************************************************
	virtual ESideOf esfSideOfHelp(const CBoundVol& bv, 
		const CPresence3<>* ppr3_this = 0, const CPresence3<>* ppr3_it = 0) const override
	{
		// Implement helper function for double dispatch.
		return bv.esfSideOf(*this, ppr3_it, ppr3_this);
	}

	virtual const CBoundVolCamera* pbvcamCast() const override { return this; }

	//
	// Helper function for other classes intersecting with CBoundVolCamera.
	//

	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	virtual CSet<EOutCode> seteocOutCodes
	(
		const CVector3<>& v3			// Point to test in volume.
	) const = 0;
	//
	// Returns:
	//		The outcodes for this point.  If non-zero, the point is outside the volume.
	//
	// Cross-references:
	//		Can be used for initial polygon rejection tests.  Also, passed to esfClipPolygonInside
	//		as a hint for which planes to clip against.
	//
	//**********************************

	//******************************************************************************************
	//
	virtual ESideOf esfClipPolygonInside
	(
		CRenderPolygon& rpoly,			// The render polygon to clip.
		CPipelineHeap& plh,				// The heap object to allocate new vertices on.
		bool b_perspective = true,		// Whether to interpolate the screen coords as projected.
		CSet<EOutCode> seteoc_poly = seteoc_esfClipPolygonInside_default_arg	
										// Previously determined outcodes, 
										// passed as a hint for optimisation.
	) const = 0;
	//
	// Identical in function to CClipPlane::bClipPolygonInside.
	//
	// Clips the polygon against this volume.  If the polygon intersects the boundary of the
	// volume, this function converts it to a new polygon lying entirely inside the volume.
	// Otherwise (it is entirely inside or outside the volume), it leaves it unchanged.
	//
	// Returns:
	//		The intersection code of the polygon with the camera; if esfINSIDE, the polygon
	//		was or is now inside the volume; if esfOUTSIDE, it is outside the volume, and unchanged.
	//
	//**********************************
};

//**********************************************************************************************
//
// Global functions return CBoundVolCameras.
//

	//**********************************************************************************************
	//
	CBoundVolCamera* pbvcamParallel
	(
		TReal r_neg_x, TReal r_pos_x,			// Boundaries of the box.
		TReal r_neg_z, TReal r_pos_z,
		TReal r_neg_y, TReal r_pos_y
	);
	//
	// Returns:
	//		A box of the given dimensions usable as a parallel camera.
	//
	//**********************************

	//**********************************************************************************************
	//
	CBoundVolCamera* pbvcamParallel
	(
		TReal r_neg_x, TReal r_pos_x,			// Boundaries of the box.
		TReal r_neg_z, TReal r_pos_z
	);
	//
	// Returns:
	//		A parallel camera with no near or far planes.  Speeds up intersection and clipping.
	//
	//**********************************

	//**********************************************************************************************
	//
	CBoundVolCamera* pbvcamParallelNorm();
	//
	// Returns:
	//		A parallel camera, with X and Z dimensions of -1 to 1, and Y dimension 0 to 1.
	//
	//**********************************

	//******************************************************************************************
	//
	CBoundVolCamera* pbvcamPerspective
	(
		TReal r_left_slope,
		TReal r_right_slope,
		TReal r_down_slope,
		TReal r_up_slope,
		TReal r_near_dist,					// Near clipping plane distance.
		TReal r_far_dist					// Far clipping plane distance.
	);
	//
	// Returns:
	//		A pyramid of the given dimensions, usable as a perspective camera.
	//
	//**********************************

	//**********************************************************************************************
	//
	CBoundVolCamera* pbvcamPerspectiveNorm
	(
		TReal r_near_dist
	);
	//
	// Returns:
	//		A perspective camera, with slopes of -1 and 1, and Y dimension from 0 to 1.
	//
	//**********************************


//******************************************************************************************
//
// CBoundVol implementation.
//

	//******************************************************************************************
	inline CBoundVolSphere CBoundVol::bvsGetBoundingSphere() const
	{
		return fMaxExtent();
	}

//
// Global functions.
//

//**********************************************************************************************
//
CBoundVol* pbvLoad
(
	int i_handle
);
//
// Returns a bounding volume loaded from a file handle.
//
//**********************************

//**********************************************************************************************
//
inline uint uGetBoxNearestCorner
(
	const CVector3<>& v3
)
//
// Returns the index of the corner closest to the given point, ie., returns the index of
// the quadrant the point is in.
//
//**************************************
{
	uint32 u4 = u4FromFloat(v3.tZ) & 0x80000000;
	u4 |= (u4FromFloat(v3.tY) & 0x80000000) >> 1;
	u4 |= u4FromFloat(v3.tX) >> 2;
	u4 >>= 29;
	return u4;
}

#endif
