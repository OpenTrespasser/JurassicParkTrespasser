/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Some flags used by GeomTypes files.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/GeomTypesPriv.hpp                                        $
 * 
 * 5     98/02/10 13:16 Speter
 * Utilise new transform / operator.
 * 
 * 4     97/08/28 18:17 Speter
 * Turned off bINTERSECT_SPHERES (too slow right now).  Removed unneeded functions.
 * 
 * 3     97/08/22 11:18 Speter
 * Made bTestSpheres work when bTEST_SPHERES is false.
 * 
 * 2     97/06/30 20:08 Speter
 * Added TransformPoints, GetWorldExtents, bTestSpheres, and pr3Total utility functions.
 * 
 * 1     97-04-14 20:43 Speter
 * Some flags needed only by GeomTypes files.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_RENDERER_GEOMTYPESPRIV_HPP
#define HEADER_LIB_RENDERER_GEOMTYPESPRIV_HPP

#define bLENIENT_INTERSECT	1
#define bTEST_SPHERES		0

//**********************************************************************************************
//
// Utility functions for bounding volume implementations.
//

	//******************************************************************************************
	//
	void GetWorldExtents
	(
		CPArray< CVector3<> > pav3_points,			// Points defining volume.
		const CTransform3<>& tf3,					// Transformation for points.
		CVector3<>& rv3_min, CVector3<>& rv3_max	// Extents to return.
	);
	//
	// Calculates the extents from the given array and transform.
	//
	//**********************************

	//**********************************************************************************************
	//
	inline bool bTestSpheres
	(
		const CBoundVol& bv_a,
		const CBoundVol& bv_b,
		const CPresence3<>* ppr3_total, 
		ESideOf* pesf_result
	)
	//
	// Returns:
	//		Whether the intersection could be determined by testing spheres.
	//
	//**************************************
	{
	#if bTEST_SPHERES
		// Intersect with bv_b's bounding sphere first.
		*pesf_result = bv_a.esfSideOf(bv_b.bvsGetBoundingSphere(), 0, ppr3_total);
		return *pesf_result != esfINTERSECT;
	#else
		return false;
	#endif
	}


	//**********************************************************************************************
	//
	inline CPresence3<> pr3Total
	(
		const CPresence3<>* ppr3_this,	// Presence of the volume space to transform into, or 0.
		const CPresence3<>* ppr3_it		// Presence of the volume space to transform out of, or 0.
	)
	//
	// Returns:
	//		The combined presence value.
	//
	//**********************************
	{
		if (ppr3_this)
		{
			if (ppr3_it)
				return *ppr3_it / *ppr3_this;
			else
				return ~*ppr3_this;
		}
		else
		{
			if (ppr3_it)
				return *ppr3_it;
			else
				return CPresence3<>();
		}
	}

#endif
