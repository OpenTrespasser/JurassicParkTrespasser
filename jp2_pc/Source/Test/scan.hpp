/*****************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *	class CScanPoly
 *
 * To do:
 *
 *****************************************************************************
 *
 * $Log: /JP2_PC/Source/Test/scan.hpp $
 * 
 * 3     96/06/26 16:44 Speter
 * Updated for new transform modules.
 * 
 * 2     96/05/07 15:21 Speter
 * Made coordinate type an additional template parameter.
 * Use integer Y instead of real Y to iterate through scanline values.
 * 
 * 1     96/04/24 12:56 Speter
 * Test files for RasterTest target.  Not style-compliant
 *
 ****************************************************************************/

#ifndef HEADER_SCAN_HPP
#define HEADER_SCAN_HPP

#include "Lib/Transform/Vector.hpp"

/*****************************************************************************/
template<class T, class Coord> class CScanPoly		// Prefix: scanp
{
public:
	//
	// Scan results.
	//
	int		iY, iX1, iX2;							// Current scanline.
	Coord	coordLeft, coordRight, coordInc;		// Coord values of scanline.

	CScanPoly(Coord acoord[], int i_cnt, int i_left, int i_top, int i_right, int i_bottom);
	bool operator ++(int);

private:
	Coord*	acoordPoly;
	int		iCount;
	T		Left, Right;				// iX clipping boundary
	int		iYmax;
};

#include "scan.cpp"

#endif
