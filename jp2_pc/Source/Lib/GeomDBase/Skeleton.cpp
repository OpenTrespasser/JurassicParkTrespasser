/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Implementation of Skeleton.hpp.
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/GeomDBase/Skeleton.cpp                                            $
 * 
 * 43    98/09/28 1:43 Speter
 * Fixed biomesh shadow bug; max extent in target space was calculated wrong.
 * 
 * 42    98.09.19 12:39a Mmouni
 * Added "forceinline" to many small inline functions.
 * Inlined some functions for speed.
 * 
 * 41    98/09/14 1:40 Speter
 * bBackface() now called, because of compiler's abominable inlining.
 * 
 * 40    9/05/98 11:09p Agrant
 * different ratios for position vs orientation for double joints
 * 
 * 39    98/08/29 19:19 Speter
 * Duuuhhhhhhhhhhhhhhhhhh
 * 
 * 38    98/08/29 18:10 Speter
 * Changed single rMaxRelativeDist to one per joint, for more efficient volumes.
 * 
 * 37    8/25/98 8:37p Rvande
 * Loop variables re-scoped
 * 
 * 36    8/13/98 11:34p Rwycko
 * Added verbose assert for assigning a skeleton to a non-Animal
 * 
 * 35    98.08.10 4:20p Mmouni
 * CBioMesh now inherits from CMeshAnimating.
 * 
 * 34    98/07/23 18:30 Speter
 * Added GetExtents() implementation, for much tighter shadowing areas.
 * 
 * 33    98/07/11 1:00 Speter
 * Removed artificial growing of CBioMesh volume. Instead, we always calculate current bounding
 * volume, and store it in CAnimate. Let CBioMesh keep its original volume, useful for physics,
 * etc.
 * 
 * 32    7/10/98 5:10p Mlange
 * Poly iterators now take a render context instead of a camera.
 * 
 * 31    98/06/30 22:15 Speter
 * Corrected pPolyIterator to actually override its inherited version (thanks, fucking C++).
 * 
 * 30    98/06/29 16:18 Speter
 * Moved CSkeletonRenderInfo out of CBioMesh and into CAnimate; also InvalidateSkeleton().
 * CBioMesh still has copies of original joints. CBioMesh makes itself relative, rather than
 * loader. Rendering functions now take the CInstance* parameter, update joints if necessary,
 * and get them from CAniamte. Removed much old code.
 * 
 * 29    98/06/26 0:23 Speter
 * Use v3Point in plPlane, which now returns actual point.
 * 
 * 28    6/19/98 2:58p Pkeet
 * Biomeshes now have the curvature flag specified.
 * 
 **********************************************************************************************/

#include "Common.hpp"
#include "Skeleton.hpp"
#include "SkeletonIterator.hpp"

#include "Lib/EntityDBase/Animate.hpp"
#include "Lib/Physics/InfoSkeleton.hpp"
#include "Lib/Renderer/Camera.hpp"
#include "Lib/Std/LocalArray.hpp"
#include "Lib/Sys/DebugConsole.hpp"
#include "Lib/Transform/TransformIO.hpp"
#include "AsmSupport.hpp"

#include <stdio.h>

//**********************************************************************************************
//
// File functions.
//

	//******************************************************************************************
	TReal rMaxColumn(const CMatrix3<>& mx3)
	{
		TReal r_xx = Sqr(mx3.v3X.tX) + Sqr(mx3.v3Y.tX) + Sqr(mx3.v3Z.tX),
			  r_yy = Sqr(mx3.v3X.tY) + Sqr(mx3.v3Y.tY) + Sqr(mx3.v3Z.tY),
			  r_zz = Sqr(mx3.v3X.tZ) + Sqr(mx3.v3Y.tZ) + Sqr(mx3.v3Z.tZ);
		return fSqrt(Max(Max(r_xx, r_yy), r_zz));
	}

//**********************************************************************************************
//
// CBioMesh implementation.
//

	//******************************************************************************************
	CBioMesh::CBioMesh
	(
		CMesh::CHeap&			mh,				// Basic mesh data.
		CPArray< CTransform3<> > patf3_joints,	// Joint info; used to make geometry relative.
		CPArray<uint>			pau_joints,		// Link info.
		CPArray<int>			pai_first_double_joints,	// The joints that the double joints hang between
		CPArray<int>			pai_second_double_joints,
		CPArray<float>			paf_position_proportion,	// Proportion of influence held by first joints
		CPArray<float>			paf_rotation_proportion		// Proportion of influence held by first joints
	) :
		// Call mesh constructor, WITHOUT merging polygons.  We need triangles for animated meshes!
		CMeshAnimating(mh, enlCURVED, false, false, true),
		patf3JointsOrig(patf3_joints),
		pauJoints(pau_joints),
		aiDoubleJointsA(pai_first_double_joints),
		aiDoubleJointsB(pai_second_double_joints),
		afPositionRatios(paf_position_proportion),
		afRotationRatios(paf_rotation_proportion),
		arMaxRelativeDist(patf3_joints.size())
	{
		// How many physics system joints do we have?
		iJoints = patf3_joints.uLen - paf_position_proportion.uLen;

		// Convert points and vertex normals to joint-relative.
		MakeRelative(patf3_joints);
	}

	//******************************************************************************************
	void CBioMesh::MakeRelative(CPArray< CTransform3<> > patf3_joints)
	{
		// Deduce joint-relative points and normals from initial geometry.
		// Also, find max relative distance.
		arMaxRelativeDist.Fill(0.0f);

		int i;
		for (i = 0; i < pav3Points.uLen; i++)
		{
			// Transform to joint space.
			pav3Points[i] /= patf3_joints[ pauJoints[i] ];

			// Find max relative dist squared for this joint.
			SetMax(arMaxRelativeDist[ pauJoints[i] ], pav3Points[i].tLenSqr());
		}

		// Square root of dists, and grow by a 1% safety margin.
		for (i = 0; i < arMaxRelativeDist.size(); i++)
			arMaxRelativeDist[i] = fSqrt(arMaxRelativeDist[i]) + 0.01f;

		for (i = 0; i < pamvVertices.uLen; i++)
		{
			int i_joint = pauJoints[ (uint)((const CVector3<> *)pamvVertices[i].pv3Point - (const CVector3<> *)pav3Points) ];

			// Transform to joint space.
			pamvVertices[i].d3Normal /= patf3_joints[i_joint];
		}
	}

	//******************************************************************************************
	void CBioMesh::GetExtents(CInstance* pins, const CTransform3<>& tf3_shape, 
		CVector3<>& rv3_min, CVector3<>& rv3_max) const
	{
		CAnimate* pani = ptCast<CAnimate>(pins);
		AlwaysAssert(pani);

		// Compute extents from centre.
		rv3_min = rv3_max = v3Zero;

		TReal r_scale = rMaxColumn(tf3_shape.mx3Mat);

		// Get extents, applying just the matrix portion of the desired transform.
		for (int i = 0; i < iJoints; i++)
		{
			CVector3<> v3_joint = pani->sriBones.patf3JointTransforms[i].v3Pos * tf3_shape.mx3Mat;

			// Adjust outward by the maximum relative vertex distance.
			// The scale of the adjustment is the maximum component of the matrix.
			TReal r_adjust = arMaxRelativeDist[i] * r_scale;

			SetMin(rv3_min.tX, v3_joint.tX - r_adjust);
			SetMin(rv3_min.tY, v3_joint.tY - r_adjust);
			SetMin(rv3_min.tZ, v3_joint.tZ - r_adjust);

			SetMax(rv3_max.tX, v3_joint.tX + r_adjust);
			SetMax(rv3_max.tY, v3_joint.tY + r_adjust);
			SetMax(rv3_max.tZ, v3_joint.tZ + r_adjust);
		}

		// Apply the offset to the results.
		rv3_min += tf3_shape.v3Pos;
		rv3_max += tf3_shape.v3Pos;
	}

	//******************************************************************************************
	uint CBioMesh::uSize() const
	{
		return CMeshAnimating::uSize() +
			patf3JointsOrig.uMemSize() +
			pauJoints.uMemSize() +
			aiDoubleJointsA.uMemSize() +
			aiDoubleJointsB.uMemSize() +
			afPositionRatios.uMemSize() +
			afRotationRatios.uMemSize();
	}

	//******************************************************************************************
	CShape::CPolyIterator* CBioMesh::pPolyIterator(const CInstance* pins, const CRenderContext* prenc) const
	{
		return new CPolyIterator(*this, pins, prenc);
	}

	//******************************************************************************************
	//
	// CBioMesh::CPolyIterator implementation.
	//

		//******************************************************************************************
		CBioMesh::CPolyIterator::CPolyIterator(const CBioMesh& bmsh, const CInstance* pins, const CRenderContext* prenc)
			: CMeshAnimating::CPolyIterator(bmsh, pins, prenc), bmshMesh(bmsh)
		{
			// We must update the bio-mesh with new joint data before rendering.
			const CPhysicsInfoSkeleton* pphis = pins->pphiGetPhysicsInfo()->ppisCast();
			if (pphis)
				// HACK: Our constness sucks. UpdateSkeleton really only needs a const instance.
				pphis->UpdateSkeleton(const_cast<CInstance*>(pins));

			// Extract joint array, and copy here.
			CAnimate* pani = ptCast<CAnimate>(const_cast<CInstance*>(pins));
#if VER_TEST
			if (!pani)
			{
				char str_buffer[256];
				sprintf(str_buffer, 
						"%s\n\nBiomesh (%s) does not have CAnimate class-  try 'string Class = \"CAnimal\";\n",
						__FILE__, 
						pins->strGetInstanceName());

				if (bTerminalError(ERROR_ASSERTFAIL, true, str_buffer, __LINE__))
					DebuggerBreak();
			}
#endif
			patf3Joints = pani->sriBones.patf3JointTransforms;
		}

		//**************************************************************************************
		void CBioMesh::CPolyIterator::TransformPoints
		(
			const CTransform3<>&	tf3_shape_camera,
			const CCamera&			cam,
			CPArray<SClipPoint>		paclpt_points,
			bool					b_outcodes
		)
		{
			//
			// Copy point array for future reference (e.g. plane equations).
			//
			paclptPoints = paclpt_points;

			// Compute combined transforms for every joint.
			CLArray(CTransform3<>, patf3_total, patf3Joints.size());
			for (int i = 0; i < patf3_total.size(); i++)
				patf3_total[i] = patf3Joints[i] * tf3_shape_camera;

			// Transform points using combined transforms.
			if (b_outcodes || VER_DEBUG)
			{
				const CBoundVolCamera* pbvcam_clip = cam.pbvcamClipVolume();

				for (int i = 0; i < bmshMesh.pav3Points.uLen; i++)
				{
					// Transform point.
					paclpt_points[i].v3Point = bmshMesh.pav3Points[i] * patf3_total[ bmshMesh.pauJoints[i] ];

					// Also generate outcode.
					paclpt_points[i].seteocOut = 
						pbvcam_clip->seteocOutCodes(paclpt_points[i].v3Point);

				#if (VER_DEBUG)
					if (paclpt_points[i].seteocOut - eocFAR)
					{
						// This point is outside the view volume, so let's make
						// sure we weren't told to disable clipping.
						// We ignore the far clipping plane for now, since if it's violated it's not
						// critical, and the render cache currently has problems staying within in.
						Assert(b_outcodes);
					}
				#endif
				}
			}
			else
			{
				// Similar procedure, but transform only, skipping clip test.
				for (int i = 0; i < bmshMesh.pav3Points.uLen; i++)
				{
					// Transform point.
					paclpt_points[i].v3Point = bmshMesh.pav3Points[i] * patf3_total[ bmshMesh.pauJoints[i] ];
				}
			}
		}

		//**************************************************************************************
		CPlane CBioMesh::CPolyIterator::plPlane()
		{
			// Currently quite slow.
			return CPlane(v3Point(0), v3Point(1), v3Point(2));
		}

//**********************************************************************************************
//
// CSubBioMesh implementation.
//

	//******************************************************************************************
	CSubBioMesh::CSubBioMesh
	(
		CMesh::CHeap&			mh,				// Basic mesh data.
		CPArray< CTransform3<> > patf3_joints,	// Joint info.
		CPArray<uint>			pau_joints,		// Link info.
		CPArray<int>			pai_first_double_joints,	// The joints that the double joints hang between
		CPArray<int>			pai_second_double_joints,
		CPArray<float>			paf_position_proportion,	// Proportion of influence held by first joints
		CPArray<float>			paf_rotation_proportion		// Proportion of influence held by first joints
	) : CBioMesh(mh, patf3_joints, pau_joints, pai_first_double_joints, pai_second_double_joints, paf_position_proportion, paf_rotation_proportion)
	{
	}

	//******************************************************************************************
	CSubBioMesh::~CSubBioMesh()
	{
		// Delete all of the substitutes.
		list<CSubstitute*>::iterator i;
		for (i = lpsubSubstitutes.begin(); i != lpsubSubstitutes.end(); ++i)
		{
			delete *i;
		}
	}

	//******************************************************************************************
	void CSubBioMesh::Substitute(const CSubstitute* psub)
	{
		// Apply the substitution.
		list<CVertexReplacement>::const_iterator i = psub->lvrVertexReplacements.begin();

		for (; i != psub->lvrVertexReplacements.end(); ++i)
		{
			// These are relative points.
			pav3Points[(*i).iIndex] = (*i).v3Position;
		}
	}

	//******************************************************************************************
	void CSubBioMesh::Substitute(int i_index)
	{
		// Find the correct CSubstitute, and apply it.
		list<CSubstitute*>::iterator i = lpsubSubstitutes.begin();

		for (; i != lpsubSubstitutes.end(); ++i)
		{
			if (i_index <= 0)
			{
				Substitute((*i));
				return;
			}
			else
				--i_index;
		}

		// Specifying a substitute that does not exist.
		Assert(false);
	}

	//******************************************************************************************
	void CSubBioMesh::MakeRelative(CSubstitute* psub, CPArray< CTransform3<> > patf3_joints)
	{
		// Calculate the relative attributes of the substitution.
		list<CVertexReplacement>::iterator i = psub->lvrVertexReplacements.begin();

		for (; i != psub->lvrVertexReplacements.end(); ++i)
		{
			// Transform to joint space.
			(*i).v3Position = (*i).v3Position / patf3_joints[ pauJoints[(*i).iIndex] ];
		}
	}



