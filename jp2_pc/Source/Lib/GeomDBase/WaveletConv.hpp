/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Contents:
 *		Contains conversions for world - quad tree units and the coeficient quantisation scalars.
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/GeomDBase/WaveletConv.hpp                                         $
 * 
 * 1     10/16/97 1:38p Mlange
 * Initial implementation.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_GEOMDBASE_WAVELETCONV_HPP
#define HEADER_LIB_GEOMDBASE_WAVELETCONV_HPP

#include "Lib/Transform/Vector.hpp"
#include "Lib/Transform/TransLinear.hpp"


struct SExportDataInfo;

namespace NMultiResolution
{
	//**********************************************************************************************
	//
	struct SMapping
	//
	// Contains conversions for world - quad tree units and the coeficient quantisation scalars.
	//
	// Prefix: mp
	//
	//**************************************
	{
		CRectangle<int> rcQuadSpaceExtents;		// Extents of quad tree, forced square and rounded to the nearest power of two.
		CRectangle<>    rcWorldSpaceExtents;	// Extents world space data.

		CTransLinear2<> tlr2QuadToWorld;		// Conversion from 2D quad tree units to world space.
		CTransLinear2<> tlr2WorldToQuad;		// Conversion from world space to 2D quad tree units.

		TReal rWorldToCoef;						// Coeficient quantisation scalar.
		TReal rCoefToWorld;						// De-quantise to world space.
		TReal rCoefToQuad;						// De-quantise to quad space.


		//******************************************************************************************
		//
		// Constructors.
		//
		SMapping()
		{
		}

		SMapping(const SExportDataInfo& dinf);
	};
};


#endif