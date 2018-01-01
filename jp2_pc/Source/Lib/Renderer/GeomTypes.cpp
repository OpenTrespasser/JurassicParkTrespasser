/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Implementation of GeomTypes.hpp.
 *
 * To do:
 *		Implement fuzziness correctly in all types.
 *		Add further Asserts using old functions.
 *		Add optional "intersect" or "contains" flag to esfSideOf functions.
 *		Maybe add optional "loose intersect" flag... or simply use the objects' bounding spheres.
 *		Maybe implement volume intersections exactly; currently, they are "loose" for boxes and
 *		polyhedra.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/GeomTypes.cpp                                            $
 * 
 * 32    98/10/01 16:36 Speter
 * Safer support for tf3 intersection.
 * 
 * 31    8/25/98 5:34p Rvande
 * MW pointer casting pickiness
 * 
 * 30    98/06/17 16:50 Speter
 * Fixed CBoundVolPoint implementation.
 * 
 * 29    98/04/17 17:57 Speter
 * Removed bCollide (obsoleted by bRayIntersect).
 * 
 * 28    98/02/10 13:16 Speter
 * Utilise new transform / operator.
 * 
 * 27    98/02/04 14:45 Speter
 * Added CBoundVolPoint.
 * 
 * 26    97/10/24 15:47 Speter
 * Added code to time transform routines.
 * 
 * 25    97/10/23 2:18p Pkeet
 * Removed the 'tf3Box' member function that takes presences and replaced it with a function
 * that takes no parameters and a function that takes a transform.
 * 
 * 24    97/10/15 6:45p Pkeet
 * Changed 'v3Collide' to 'bCollide.'
 * 
 * 23    97/10/15 1:29a Pkeet
 * Added the 'v3Collide' member function.
 * 
 * 22    97/10/10 12:28p Pkeet
 * Maded the 'tf3Box' function accept presence arguments. Added an 'esfSideOf' that worked with
 * a box transform.
 * 
 * 21    97/10/07 18:12 Speter
 * Changed rGetVolume() to take a scale rather than a whole presence.
 * 
 * 20    97/09/29 16:22 Speter
 * All bounding volumes are now positionless.  Removed v3Pos from Box and Sphere.  Removed
 * v3GetOrigin().
 * 
 * 19    97/09/23 21:17 Speter
 * Fixed Release build.
 * 
 * 18    97/09/23 7:09p Pkeet
 * Added load and save functions.
 * 
 **********************************************************************************************/

#include "Common.hpp"
#include "GeomTypes.hpp"
#include "GeomTypesPriv.hpp"

#include <fcntl.h>
#include <io.h>
#include <stdio.h>

#define bTEST_GEOM			VER_DEBUG
#define bTIME_TRANSFORMS	0

//
// Rationale:	Intersect/containment flag in one function.
//				Making plane a BV allows use of planes for partitioning.
//				Plane and BV's already shared much functionality, including intersection tests.
//				More accurate intersection tests.
//

//
// Here we store a useful table splaining exactly how volume intersections are performed.
//
/*
				GENERAL	Infin	Plane	Sphere	Box		Polyh	Compoun	v3		Polygon
	HELP				I		V^L		V^S		V^B		V^P		*
	GENERAL				I						(vert)	(vert)	vols
	Infinite	I		IO		I		I		I		I		I		I		I
	Plane				IO		IO		*		(^^)	(^^)	^^		*		V^G
	Sphere				IO		(L^S)	*		(B^S)	(P^S)	^^		*		V^G
	Box			(plns)	IO		(L^B)	*		(^^)	(^^)	^^		*		V^G
	Polyh		(plns)	IO		(L^P)	<<		(^^)	(^^)	^^		*		V^G
	Camera		(plns)	IO		(L^C)	<<		(^^)	(^^)			*		
	Compound	vols	IO		<<		<<		<<		<<		<<		*		*
*/

//**********************************************************************************************
//
// CBoundVol implementation.
//

	//*****************************************************************************************
	void CBoundVol::Save(int i_handle) const
	{
		int i_num_written;
		EBVType ebv = ebvGetType();

		// Store the type of bounding volume represented.
		i_num_written = _write(i_handle, (char*)&ebv, (int)sizeof(ebv));
		AlwaysAssert(i_num_written == sizeof(ebv));
	}


//**********************************************************************************************
//
// CBoundVol implementation.
//

	//******************************************************************************************
	TReal CBoundVol::rGetVolume(TReal r_scale) const
	{
		TReal r_radius = bvsGetBoundingSphere().rRadius * r_scale;
		return 4.0f / 3.0f * 3.14159f * r_radius * r_radius * r_radius;
	}
	
	//******************************************************************************************
	CTransform3<> CBoundVol::tf3Box() const
	{
		Assert(0);
		CTransform3<> tr3;
		return tr3;
	}

	//******************************************************************************************
	CTransform3<> CBoundVol::tf3Box(const CTransform3<>& tr3) const
	{
		Assert(0);
		return tr3;
	}
	
	//******************************************************************************************
	ESideOf CBoundVol::esfSideOf(const CTransform3<>& tf3_box) const
	{
		Assert(0);
		return esfINTERSECT;
	}

	//******************************************************************************************
	ESideOf CBoundVol::esfSideOf(const CBoundVolPoint& bvpt, 
		const CPresence3<>* ppr3_this, const CPresence3<>* ppr3_it) const
	{
		if (ppr3_this)
		{
			if (ppr3_it)
				return esfSideOf(ppr3_it->v3Pos / *ppr3_this);
			else
				return esfSideOf((~*ppr3_this).v3Pos);
		}
		else
		{
			if (ppr3_it)
				return esfSideOf(ppr3_it->v3Pos);
			else
				return esfSideOf(v3Zero);
		}
	}

//**********************************************************************************************
//
// CBoundVolPoint implementation.
//

	//******************************************************************************************
	ESideOf CBoundVolPoint::esfSideOf(const CVector3<>& v3) const
	{
		return esfOUTSIDE;
	}

	//******************************************************************************************
	ESideOf CBoundVolPoint::esfSideOf(CPArray< CVector3<> > pav3_solid) const
	{
		return esfOUTSIDE;
	}

	//
	// Universal function for point intersection.
	// If the volume contains the point, return esfINTERSECT, else esfOUTSIDE.
	//

	//******************************************************************************************
	ESideOf esfSideOfPoint(const CBoundVol& bv, 
		const CPresence3<>* ppr3_this = 0, const CPresence3<>* ppr3_it = 0)
	{
		// This is the hard part. Figure out the point to use, in bv's space.
		if (ppr3_this)
		{
			if (ppr3_it)
				return bv.esfSideOf(ppr3_this->v3Pos / *ppr3_it) == esfOUTSIDE ? esfOUTSIDE : esfINTERSECT;
			else
				return bv.esfSideOf(ppr3_this->v3Pos) == esfOUTSIDE ? esfOUTSIDE : esfINTERSECT;
		}
		else
		{
			if (ppr3_it)
				return bv.esfSideOf(ppr3_it->v3Pos / ppr3_it->r3Rot / -ppr3_it->rScale) == esfOUTSIDE ? esfOUTSIDE : esfINTERSECT;
			else
				return bv.esfSideOf(v3Zero) == esfOUTSIDE ? esfOUTSIDE : esfINTERSECT;
		}
	}

	//******************************************************************************************
	ESideOf CBoundVolPoint::esfSideOf(const CBoundVol& bv, 
		const CPresence3<>* ppr3_this, const CPresence3<>* ppr3_it) const
	{
		return esfSideOfPoint(bv, ppr3_this, ppr3_it);
	}

	//******************************************************************************************
	ESideOf CBoundVolPoint::esfSideOf(const CBoundVolInfinite& bvi, 
		const CPresence3<>* ppr3_this, const CPresence3<>* ppr3_it) const
	{
		return esfINTERSECT;
	}

	//******************************************************************************************
	ESideOf CBoundVolPoint::esfSideOf(const CBoundVolPoint& bvpt, 
		const CPresence3<>* ppr3_this, const CPresence3<>* ppr3_it) const
	{
		return esfOUTSIDE;
	}

	//******************************************************************************************
	ESideOf CBoundVolPoint::esfSideOf(const CBoundVolSphere& bvs, 
		const CPresence3<>* ppr3_this, const CPresence3<>* ppr3_it) const
	{
		return esfSideOfPoint(bvs, ppr3_this, ppr3_it);
	}

	//******************************************************************************************
	ESideOf CBoundVolPoint::esfSideOf(const CBoundVolPlane& bvpl, 
		const CPresence3<>* ppr3_this, const CPresence3<>* ppr3_it) const
	{
		return esfSideOfPoint(bvpl, ppr3_this, ppr3_it);
	}

	//******************************************************************************************
	ESideOf CBoundVolPoint::esfSideOf(const CBoundVolBox& bvb, 
		const CPresence3<>* ppr3_this, const CPresence3<>* ppr3_it) const
	{
		return esfSideOfPoint(bvb, ppr3_this, ppr3_it);
	}

	//******************************************************************************************
	ESideOf CBoundVolPoint::esfSideOf(const CBoundVolPolyhedron& bvp, 
		const CPresence3<>* ppr3_this, const CPresence3<>* ppr3_it) const
	{
		return esfSideOfPoint(bvp, ppr3_this, ppr3_it);
	}

	//******************************************************************************************
	ESideOf CBoundVolPoint::esfSideOf(const CBoundVolCamera& bvcam, 
		const CPresence3<>* ppr3_this, const CPresence3<>* ppr3_it) const
	{
		return esfSideOfPoint(bvcam, ppr3_this, ppr3_it);
	}


//**********************************************************************************************
//
// Global functions.
//

	//******************************************************************************************
	void GetWorldExtents(CPArray< CVector3<> > pav3_points, const CTransform3<>& tf3,
		CVector3<>& rv3_min, CVector3<>& rv3_max)
	{
		// Transform all points, and find limits.

		// Set initial values to point 0 (origin) transformed by presence.
		rv3_min = rv3_max = pav3_points[0] * tf3;

		// Compare with other points.
		for (int i = 1; i < pav3_points.uLen; i++)
		{
			CVector3<> v3 = pav3_points[i] * tf3;

			rv3_min.SetMin(v3);
			rv3_max.SetMax(v3);
		}
	}

	//**********************************************************************************************
	CBoundVol* pbvLoad(int i_handle)
	{
		EBVType ebv;

		// Read in the type of bounding volume represented.
		int i_num_read = _read(i_handle, (char*)&ebv, (int)sizeof(ebv));
		Assert(i_num_read == sizeof(ebv));

		switch (ebv)
		{
			case ebvINFINITE:
				return new CBoundVolInfinite();
			case ebvSPHERE:
				return new CBoundVolSphere(i_handle);
			case ebvBOX:
				return new CBoundVolBox(i_handle);
			default:
				Assert(0);
				return new CBoundVolInfinite();
		}
	}


#if bTEST_GEOM || bTIME_TRANSFORMS

//
// Utility functions.
//

#include "Lib/Std/Random.hpp"

static CRandom rando;

float fRandom()
{
	return rando(-100.0, 100.0);
}

float fPosRandom()
{
	return rando(0.1, 100.0);
}

CVector3<> v3Random()
{
	return CVector3<>(fRandom(), fRandom(), fRandom());
}

CVector3<> v3PosRandom()
{
	return CVector3<>(fPosRandom(), fPosRandom(), fPosRandom());
}

CPresence3<> pr3Random()
{
	return CPresence3<>(CRotate3<>(v3Random()), fPosRandom(), v3Random());
}

#endif

#if bTEST_GEOM

//******************************************************************************************
//
class CBoundVolTester
//
// Test class.
//
//**************************************
{
	CRandom rand;

public:
	CBoundVolTester()
	{
		for (int i = 0; i < 100; i++)
		{
			// Construct a bunch of random bounding volumes, and test their intersection code.
			// Currently ignore compound volume stuff.
//			TestBoundVol(CBoundVolInfinite());
			TestBoundVol(bvplPlaneRandom());
			TestBoundVol(bvsSphereRandom());
			TestBoundVol(bvbBoxRandom());
			TestBoundVol(bvpPolyhedronRandom());

			aptr<CBoundVolCamera> pbvcam = pbvcamCameraPerspectRandom();
			TestBoundVol(*pbvcam);
		}
	}

	void TestBoundVol(const CBoundVol& bv)
	{
		TestBoundVols(bv, CBoundVolInfinite());
		TestBoundVols(bv, bvplPlaneRandom());
		TestBoundVols(bv, bvsSphereRandom());
		TestBoundVols(bv, bvbBoxRandom());
		TestBoundVols(bv, bvpPolyhedronRandom());

		aptr<CBoundVolCamera> pbvcam = pbvcamCameraPerspectRandom();
		TestBoundVols(bv, *pbvcam);
	}

	void TestBoundVols(const CBoundVol& bv1, const CBoundVol& bv2)
	{
		IntersectBoundVols(bv1, bv2);

		CPresence3<> pr3_1 = pr3Random();
		CPresence3<> pr3_2 = pr3Random();

		IntersectBoundVols(bv1, bv2, &pr3_1, &pr3_2);
	}

	void IntersectBoundVols(const CBoundVol& bv1, const CBoundVol& bv2,
		const CPresence3<>* pr3_1 = 0, const CPresence3<>* pr3_2 = 0)
	{
		// Just perform some minimal consistency checking.
		ESideOf esf_1 = bv1.esfSideOf(bv2, pr3_1, pr3_2);

		// Consistency check with bounding spheres.
		ESideOf esf_s1 = bv1.bvsGetBoundingSphere().esfSideOf(bv2, pr3_1, pr3_2);
		ESideOf esf_s2 = bv1.esfSideOf(bv2.bvsGetBoundingSphere(), pr3_1, pr3_2);
		ESideOf esf_ss = bv1.bvsGetBoundingSphere().esfSideOf(bv2.bvsGetBoundingSphere(), pr3_1, pr3_2);

		// If A is inside B, then B intersects A 
		// (unless it's identical with it; a possibility we ignore for these tests).
		ESideOf esf_2 = bv2.esfSideOf(bv1, pr3_2, pr3_1);

		switch (esf_1)
		{
			case esfINSIDE:
				Assert(esf_s1 == esfINSIDE);
				Assert(esf_s2 & esfINSIDE);
				Assert(esf_ss & esfINSIDE);
				Assert(esf_2 == esfINTERSECT);
				break;
			case esfOUTSIDE:
				Assert(esf_s2 != esfINSIDE);
#if bLENIENT_INTERSECT
				Assert(esf_2 & esfOUTSIDE);
#else
				Assert(esf_2 == esfOUTSIDE);
#endif
				break;
			case esfINTERSECT:
#if bLENIENT_INTERSECT
				Assert(esf_s2 != esfINSIDE);
#else
				Assert(esf_s1 & esfINSIDE);
				Assert(esf_s2 == esfINTERSECT);
				Assert(esf_ss & esfINSIDE);
				Assert(esf_2 & esfINSIDE);
#endif
				break;
			default:
				Assert(false);
				break;
		}
	}

	void CompareBoundVols(const CBoundVol& bv_1a, const CBoundVol& bv_1b)
	{
		CompareVolumes(bv_1a, bv_1b, CBoundVolInfinite());
		CompareVolumes(bv_1a, bv_1b, bvplPlaneRandom());
		CompareVolumes(bv_1a, bv_1b, bvsSphereRandom());
		CompareVolumes(bv_1a, bv_1b, bvbBoxRandom());
		CompareVolumes(bv_1a, bv_1b, bvpPolyhedronRandom());
	}

	void CompareVolumes(const CBoundVol& bv_1a, const CBoundVol& bv_1b, const CBoundVol& bv_2)
	{
		IntersectVolumes(bv_1a, bv_1b, bv_2);

		CPresence3<> pr3_1 = pr3Random();
		CPresence3<> pr3_2 = pr3Random();

		IntersectVolumes(bv_1a, bv_1b, bv_2, &pr3_1, &pr3_2);
	}

	void IntersectVolumes(const CBoundVol& bv_1a, const CBoundVol& bv_1b, const CBoundVol& bv_2,
		const CPresence3<>* pr3_1 = 0, const CPresence3<>* pr3_2 = 0)
	{
		ESideOf esf_a = bv_1a.esfSideOf(bv_2, pr3_1, pr3_2);
		ESideOf esf_b = bv_1b.esfSideOf(bv_2, pr3_1, pr3_2);

		Assert(esf_a == esf_b);

		esf_a = bv_2.esfSideOf(bv_1a, pr3_1, pr3_2);
		esf_b = bv_2.esfSideOf(bv_1b, pr3_1, pr3_2);

		Assert(esf_a == esf_b);
	}

	CBoundVolSphere bvsSphereRandom()
	{
		return CBoundVolSphere(fPosRandom());
	}

	CBoundVolPlane bvplPlaneRandom()
	{
		return CBoundVolPlane(CPlane(v3Random(), v3Random(), v3Random()));
	}

	CBoundVolBox bvbBoxRandom()
	{
		return CBoundVolBox(v3PosRandom());
	}

	CBoundVolPolyhedron bvpPolyhedronRandom()
	{
		// For now, let's just construct a tetrahedron.
		CVector3<> av3_points[4] = {v3Random(), v3Random(), v3Random(), v3Random()};
		CPlane     apl_planes[4] = 
		{
			CPlane(av3_points[1], av3_points[2], av3_points[3]),
			CPlane(av3_points[2], av3_points[3], av3_points[0]),
			CPlane(av3_points[3], av3_points[0], av3_points[1]),
			CPlane(av3_points[0], av3_points[1], av3_points[2])
		};

		// Make sure all the planes face outward; away from the other point.
		for (int i = 0; i < 4; i++)
		{
			if (apl_planes[i].esfSideOf(av3_points[i]) == esfOUTSIDE)
			{
				// Reverse it.
				apl_planes[i].d3Normal = - apl_planes[i].d3Normal;
				apl_planes[i].rD = - apl_planes[i].rD;
			}
		}
		return CBoundVolPolyhedron(PArray(4, apl_planes).paDup(), PArray(4, av3_points).paDup());
	}

	CBoundVolCamera* pbvcamCameraPerspectRandom()
	{
		return pbvcamPerspective
		(
			rand(-2.0, 0), rand(0, 2.0),
			rand(-2.0, 0), rand(0, 2.0),
			rand(0.0, 0.1), rand(1.0, 100.0)
		);
	};

};

// Test object.
static CBoundVolTester BoundVolTester;

#endif


#if bTIME_TRANSFORMS

#include "Lib/Sys/Profile.hpp"
#include "Lib/Sys/DebugConsole.hpp"

//******************************************************************************************
//
class CGeomTimer
//
//**************************************
{
public:
	CGeomTimer()
	{
		CProfileStatMain psMain;
		psMain.Add(0, 1);

		CProfileStat psVecMatrix("v3 * mx3", &psMain);
		CProfileStat psVecTransform("v3 * tf3", &psMain);
		CProfileStat psVecRotate("v3 * r3", &psMain);
		CProfileStat psVecPlacement("v3 * p3", &psMain);
		CProfileStat psVecPresence("v3 * pr3", &psMain);

		CProfileStat psDirMatrix("d3 * mx3", &psMain);
		CProfileStat psDirTransform("d3 * tf3", &psMain);
		CProfileStat psDirRotate("d3 * r3", &psMain);
		CProfileStat psDirPlacement("d3 * p3", &psMain);
		CProfileStat psDirPresence("d3 * pr3", &psMain);

		CProfileStat psMatrix("mx3 * mx3", &psMain);
		CProfileStat psTransform("tf3 * tf3", &psMain);
		CProfileStat psRotate("r3 * r3", &psMain);
		CProfileStat psPlacement("p3 * p3", &psMain);
		CProfileStat psPresence("pr3 * pr3", &psMain);

		CProfileStat psMatrixRotate("mx3 = r3", &psMain);
		CProfileStat psTransformPlacement("tf3 = p3", &psMain);
		CProfileStat psTransformPresence("tf3 = pr3", &psMain);

		CProfileStat psMatrixInv("~mx3", &psMain);
		CProfileStat psTransformInv("~tf3", &psMain);
		CProfileStat psRotateInv("~r3", &psMain);
		CProfileStat psPlacementInv("~p3", &psMain);
		CProfileStat psPresenceInv("~pr3", &psMain);

		const int iREP = 1000, iREP_TOTAL = 10;

		CVector3<> av3[iREP];
		CDir3<> ad3[iREP];
		CPresence3<> apr3[iREP];
		CPlacement3<> ap3[iREP];
		CRotate3<> ar3[iREP];
		CTransform3<> atf3[iREP];
		CMatrix3<> amx3[iREP];

		for (int i = 0; i < iREP; i++)
		{
			av3[i] = v3Random();
			ad3[i] = av3[i];
			apr3[i] = pr3Random();
			ap3[i] = apr3[i];
			ar3[i] = apr3[i].r3Rot;
			atf3[i] = apr3[i];
			amx3[i] = atf3[i].mx3Mat;
		}

		CVector3<> av3_t[iREP];
		CDir3<> ad3_t[iREP];
		CMatrix3<> amx3_t[iREP];
		CTransform3<> atf3_t[iREP];
		CRotate3<> ar3_t[iREP];
		CPlacement3<> ap3_t[iREP];
		CPresence3<> apr3_t[iREP];

		for (int i_loop = 0; i_loop < iREP_TOTAL; i_loop++)
		{
			CCycleTimer	ctmr;
			int i;

			// Vector.
			for (i = 0; i < iREP; i++)
				av3_t[i] = av3[i] * amx3[i];
			psVecMatrix.Add(ctmr(), iREP);

			for (i = 0; i < iREP; i++)
				av3_t[i] = av3[i] * atf3[i];
			psVecTransform.Add(ctmr(), iREP);

			for (i = 0; i < iREP; i++)
				av3_t[i] = av3[i] * ar3[i];
			psVecRotate.Add(ctmr(), iREP);

			for (i = 0; i < iREP; i++)
				av3_t[i] = av3[i] * ap3[i];
			psVecPlacement.Add(ctmr(), iREP);

			for (i = 0; i < iREP; i++)
				av3_t[i] = av3[i] * apr3[i];
			psVecPresence.Add(ctmr(), iREP);

			// Dir.
			for (i = 0; i < iREP; i++)
				ad3_t[i] = ad3[i] * amx3[i];
			psDirMatrix.Add(ctmr(), iREP);

			for (i = 0; i < iREP; i++)
				ad3_t[i] = ad3[i] * atf3[i];
			psDirTransform.Add(ctmr(), iREP);

			for (i = 0; i < iREP; i++)
				ad3_t[i] = ad3[i] * ar3[i];
			psDirRotate.Add(ctmr(), iREP);

			for (i = 0; i < iREP; i++)
				ad3_t[i] = ad3[i] * ap3[i];
			psDirPlacement.Add(ctmr(), iREP);

			for (i = 0; i < iREP; i++)
				ad3_t[i] = ad3[i] * apr3[i];
			psDirPresence.Add(ctmr(), iREP);

			// Self.
			for (i = 0; i < iREP; i++)
				amx3_t[i] = amx3[i] * amx3[i];
			psMatrix.Add(ctmr(), iREP);

			for (i = 0; i < iREP; i++)
				atf3_t[i] = atf3[i] * atf3[i];
			psTransform.Add(ctmr(), iREP);

			for (i = 0; i < iREP; i++)
				ar3_t[i] = ar3[i] * ar3[i];
			psRotate.Add(ctmr(), iREP);

			for (i = 0; i < iREP; i++)
				ap3_t[i] = ap3[i] * ap3[i];
			psPlacement.Add(ctmr(), iREP);

			for (i = 0; i < iREP; i++)
				apr3_t[i] = apr3[i] * apr3[i];
			psPresence.Add(ctmr(), iREP);

			// Convert.
			for (i = 0; i < iREP; i++)
				amx3_t[i] = ar3[i];
			psMatrixRotate.Add(ctmr(), iREP);

			for (i = 0; i < iREP; i++)
				atf3_t[i] = ap3[i];
			psTransformPlacement.Add(ctmr(), iREP);

			for (i = 0; i < iREP; i++)
				atf3_t[i] = apr3[i];
			psTransformPresence.Add(ctmr(), iREP);

			// Inverse.
			for (i = 0; i < iREP; i++)
				amx3_t[i] = ~amx3[i];
			psMatrixInv.Add(ctmr(), iREP);

			for (i = 0; i < iREP; i++)
				atf3_t[i] = ~atf3[i];
			psTransformInv.Add(ctmr(), iREP);

			for (i = 0; i < iREP; i++)
				ar3_t[i] = ~ar3[i];
			psRotateInv.Add(ctmr(), iREP);

			for (i = 0; i < iREP; i++)
				ap3_t[i] = ~ap3[i];
			psPlacementInv.Add(ctmr(), iREP);

			for (i = 0; i < iREP; i++)
				apr3_t[i] = ~apr3[i];
			psPresenceInv.Add(ctmr(), iREP);
		}

		CStrBuffer strbuf(4000);
		psMain.WriteToBuffer(strbuf);

		dout << strbuf;
	}
};

static CGeomTimer GeomTimer;

#endif