/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Implementation of WaveletConv.hpp.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/GeomDBase/WaveletConv.cpp                                         $
 * 
 * 3     10/24/97 6:48p Mlange
 * Now uses quantisation value from SExportDataInfo structure.
 * 
 * 2     10/24/97 4:31p Mlange
 * Added iNUM_COEF_BITS definition.
 * 
 * 1     10/16/97 1:38p Mlange
 * Initial implementation.
 * 
 **********************************************************************************************/

#include "gblinc/common.hpp"
#include "WaveletConv.hpp"

#include "Lib/GeomDBase/TerrainLoad.hpp"
#include "Lib/GeomDBase/WaveletCoef.hpp"


namespace NMultiResolution
{

//**********************************************************************************************
//
// NMultiResolution::SMapping implementation.
//

	//******************************************************************************************
	SMapping::SMapping(const SExportDataInfo& edi)
		: rcWorldSpaceExtents(edi.v2Min, CVector2<>(edi.iMaxIndices * edi.rHighestRes, edi.iMaxIndices * edi.rHighestRes)),
		  rcQuadSpaceExtents(0, 0, edi.iMaxIndices, edi.iMaxIndices)
	{
		tlr2QuadToWorld = CTransLinear2<>(rcConvert(TReal(), rcQuadSpaceExtents), rcWorldSpaceExtents);
		tlr2WorldToQuad = ~tlr2QuadToWorld;

		Assert(rcConvert(TReal(), rcQuadSpaceExtents).v2End() * tlr2QuadToWorld == rcWorldSpaceExtents.v2End());

		// Determine the quantisation transform.
		TReal r_abs_max = Max(Abs(edi.rMinHeight), Abs(edi.rMaxHeight));

		int i_max_coef = 1 << edi.iNumQuantisationBits;

		rWorldToCoef = i_max_coef   / r_abs_max;
		rCoefToWorld = r_abs_max    / i_max_coef;
		rCoefToQuad  = rCoefToWorld / edi.rHighestRes;
	}
};
