/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		CSkeletonRenderInfo
 *
 * Bugs:
 *
 * To do:
 *		Rename CSkeletonRenderInfo
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/source/Lib/GeomDBase/Skeleton.hpp                                            $
 * 
 * 37    9/25/98 1:51a Pkeet
 * Added an overloaded 'pav3GetWrap' member function to return a null array.
 * 
 * 36    98.09.19 12:39a Mmouni
 * Added "forceinline" to many small inline functions.
 * Inlined some functions for speed.
 * 
 * 35    9/05/98 11:09p Agrant
 * different ratios for position vs orientation for double joints
 * 
 * 34    98/08/29 18:10 Speter
 * Changed single rMaxRelativeDist to one per joint, for more efficient volumes.
 * 
 * 33    98.08.10 4:20p Mmouni
 * CBioMesh now inherits from CMeshAnimating.
 * 
 * 32    98/07/23 18:30 Speter
 * Added GetExtents() implementation.
 * 
 * 31    98/07/11 0:59 Speter
 * Removed artificial growing of CBioMesh volume. Instead, we always calculate current bounding
 * volume, and store it in CAnimate.
 * 
 * 30    7/10/98 5:10p Mlange
 * Poly iterators now take a render context instead of a camera.
 * 
 * 29    98/06/30 22:15 Speter
 * Corrected pPolyIterator to actually override its inherited version (thanks, fucking C++).
 * 
 * 28    98/06/29 16:15 Speter
 * Moved CSkeletonRenderInfo out of CBioMesh and into CAnimate; also InvalidateSkeleton().
 * CBioMesh still has copies of original joints. CBioMesh makes itself relative, rather than
 * loader. Rendering functions now take the CInstance* parameter. Removed much old code.
 * 
 * 27    98/06/07 19:04 Speter
 * Bio-iterator now always used. Removed relative point and normal arrays; now stuff original
 * mesh with relative values. Transforms now only updated during rendering. Added joint length
 * extraction functions.
 * 
 * 26    6/03/98 8:08p Pkeet
 * Changed 'Render' to accept the 'CShapePresence' variable.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_GEOMDBASE_SKELETON_HPP
#define HEADER_LIB_GEOMDBASE_SKELETON_HPP

#include "Mesh.hpp"

#include <list>

class CInstance;
class CShapePresence;

//**********************************************************************************************
//
class CBioMesh : public CMeshAnimating
//
// Prefix: bmsh
//
//	A RenderType class that supports skeletal biomodels with skin.
//
//**************************************
{
	//******************************************************************************************
	//
	// Data members.
	//

public:
	CAArray< CTransform3<> > patf3JointsOrig;		// The original joints for this mesh.
													// Invalid after construction, as this mesh may be
													// shared among different animals, each of whom has
													// different joint positions.
													// This is stored here as an aid to constructing
													// the CAnimate object.

	//
	// The following data are used to update geometry from new joint transforms.
	// They are intimately linked to the SkeletonRenderInfo in an Animate instance,
	// which contains per-instance joint geometry.
	//
	CAArray<uint>			pauJoints;				// Array linking joints'n'points.
	int						iJoints;				// Number of physics-filled joints.
													// This number is different from the total num
													// of joints by the number of double joints.
			
	// A is first joint used to determine double joint PosRot, B is second.
	CAArray<int>			aiDoubleJointsA;		// Joints not specified by physics, but are instead
	CAArray<int>			aiDoubleJointsB;		// interpolated between two physics joints.
													// Note that apiDoubleJoints[n] indexespauJoints[n+iJoints]

	CAArray<float>			afPositionRatios;		// fRatios[n] is the Proportion of control that
													// joint apiDoubleJoints[n].first exerts over the double joint.
	CAArray<float>			afRotationRatios;		// fRatios[n] is the Proportion of control that
													// joint apiDoubleJoints[n].first exerts over the double joint.

	CAArray<TReal>			arMaxRelativeDist;		// Maximum distance of any vertex from each joint (object space).

public:
	//******************************************************************************************
	//
	// Constructors and destructor.
	//

	CBioMesh
	(
		CMesh::CHeap&			mh,					// Basic mesh data.
		CPArray< CTransform3<> > patf3_joints,		// Joint info.
		CPArray<uint>			pau_joints,			// Link info.
		CPArray<int>			pai_first_double_joints,	// The joints that the double joints hang between
		CPArray<int>			pai_second_double_joints,
		CPArray<float>			paf_position_proportion,	// Proportion of influence held by first joints
		CPArray<float>			paf_rotation_proportion		// Proportion of influence held by first joints
	);
	//
	// Initialise the biomesh with initial geometry, and joint mappings.  From this, 
	// the joint-relative points and normals are deduced.
	//

	virtual ~CBioMesh()
	{
	}

	//******************************************************************************************
	//
	// Overrides.
	//

	//******************************************************************************************
	virtual void GetExtents(CInstance* pins, const CTransform3<>& tf3_shape,
		CVector3<>& rv3_min, CVector3<>& rv3_max) const override;

	//
	// Inherit Render and CPolyIterator.
	//

	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	uint uSize() const;
	//
	// Returns:
	//		Size of biomesh in bytes (not counting textures).
	//
	//**********************************

	//******************************************************************************************
	virtual void Render
	(
		const CInstance*			pins,				// The instance owning this shape.
		CRenderContext&				renc,				// The rendering context.
		CShapePresence&				rsp,				// The shape-to-world transform.
		const CTransform3<>&		tf3_shape_camera,	// The shape to camera transform.
		const CPArray<COcclude*>&	papoc,				// Array of occluding objects.
		ESideOf						esf_view			// Shape's relation to the view volume
														// (for trivial acceptance).
	) const override;

	//
	// Forward nested class declaration; include "SkeletonIterator.hpp" for definition.
	//
	class CPolyIterator;

	//******************************************************************************************
	virtual CShape::CPolyIterator* pPolyIterator(const CInstance* pins, const CRenderContext* prenc) const override;

	//******************************************************************************************
	virtual rptr_const<CBioMesh> rpbmCast() const override
	{
		return rptr_const_this(this);
	}

	//******************************************************************************************
	virtual CPArray< CVector3<> > pav3GetWrap() const override
	{
		return CPArray< CVector3<> >(0, 0);
	}

public:
	//  Cast override:

	//*****************************************************************************************
	virtual void Cast(rptr_const<CBioMesh>* ppbm) const override
	{
		*ppbm = rptr_const_this(this);
	}

	friend CPolyIterator;

protected:

	//******************************************************************************************
	//
	void MakeRelative
	(
		CPArray< CTransform3<> > patf3_joints		// Joints for this biomesh.
	);
	//
	// Converts mesh point and vertex normal data to joint-relative values, for use in 
	// CBioMesh::CPolyIterator
	//
	//**********************************
};


//**********************************************************************************************
//
class CVertexReplacement
//
// Prefix: vr
//
//	A single vertex replacement, containing index and vector.
//
//**************************************
{
public:
	int iIndex;				// Index of the vertex to replace.
	CVector3<> v3Position;	// Location of the replaced vertex.
};

//**********************************************************************************************
//
class CSubstitute
//
// Prefix: sub
//
//	A set of vertex info that defines a new shape for a SubBioMesh
//
//**************************************
{
public:
	std::list<CVertexReplacement> lvrVertexReplacements;		// The substitute vertex positions.
															// index, vector
};

//**********************************************************************************************
//
class CSubBioMesh : public CBioMesh
//
// Prefix: sbmsh
//
//	A RenderType class that supports skeletal biomodels with skin and substitute vertex positioning
//
//**************************************
{
	//******************************************************************************************
	//
	// Data members.
	//
public:

	std::list<CSubstitute*> lpsubSubstitutes;		// The substitute vertex positions.

public:
	//******************************************************************************************
	//
	// Constructors and destructor.
	//

	CSubBioMesh
	(
		CMesh::CHeap&			mh,				// Basic mesh data.
		CPArray< CTransform3<> > patf3_joints,	// Joint info.
		CPArray<uint>			pau_joints,		// Link info.
		CPArray<int>			pai_first_double_joints,	// The joints that the double joints hang between
		CPArray<int>			pai_second_double_joints,
		CPArray<float>			paf_pos_proportion,			// Proportion of influence held by first joints
		CPArray<float>			paf_rot_proportion			// Proportion of influence held by first joints
	);
	//
	// Initialise the biomesh with initial geometry, and joint mappings.  From this, 
	// the joint-relative points and normals are deduced.
	//


	virtual ~CSubBioMesh();

	//******************************************************************************************
	//
	void Substitute
	(
		const CSubstitute* psub		// New vertex substitution.
	);		
	//
	// Alters the mesh to match the substitution
	//
	//**********************************

	//******************************************************************************************
	//
	void Substitute
	(
		int i_index
	);		
	//
	// Alters the mesh to match the substitution indexed by "i_index"
	//
	//**********************************

	//******************************************************************************************
	//
	void MakeRelative
	(
		CSubstitute* psub,				// New vertex substitution.
		CPArray< CTransform3<> > patf3_joints	// Joints for this biomesh.
	);		
	//
	// Alters the substitute so that it's positions are joint-relative rather than world-relative.
	//
	//**********************************
};



//#ifndef HEADER_LIB_GEOMDBASE_SKELETON_HPP
#endif
