/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Implementation of scan.hpp.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Test/scan.cpp                                                         $
 * 
 * 7     96/07/12 18:02 Speter
 * Added Z checking to scanline loop.
 * 
 * 6     96/06/26 16:44 Speter
 * Updated for new transform modules.
 * 
 * 5     96/05/23 9:42 Speter
 * Added casts to avoid warnings.
 * 
 * 4     96/05/13 14:21 Speter
 * Changed bWithin call to bBetween
 * 
 * 3     96/05/07 15:21 Speter
 * Made coordinate type an additional template parameter.
 * Use integer Y instead of real Y to iterate through scanline values.
 * 
 * 2     96/04/24 12:55 Speter
 * Test files for RasterTest project now converted to work in our common code base.  Still not
 * style-compliant.
 * 
 * 1     96/04/23 16:22 Speter
 * These are all test files to help test raster, geom, etc.
 * They are not JP2PC-style compatible.
 * 
 **********************************************************************************************/

#include <math.h>

#include "common.hpp"
#include "scan.hpp"

/*************************************************************************
 * CScanPoly implementation
 *************************************************************************/

	template<class T, class Coord> CScanPoly<T, Coord>::CScanPoly(Coord acoord[], int i_cnt, int i_left, int i_top, int i_right, int i_bottom) 
	/*
	 * Constructor for the iterator.  Sets structure up for iteration by saving
	 * relevant initialisation parameters, and calculating min and max Y.
	 */
	{
		// Initialise struct.
		acoordPoly = acoord;
		iCount = i_cnt;

		// Copy clip coords in floating point.
		Left = (T) i_left;
		Right = (T) i_right;

		// Find min and max Y.
		T t_y = acoordPoly[0].tY;
		T t_ymax = acoordPoly[0].tY;

		for (int i = 1; i < iCount; i++) 
		{
			SetMin(t_y, acoordPoly[i].tY);
			SetMax(t_ymax, acoordPoly[i].tY);
		}

		//
		// Set Y and Ymax to integer values.
		//
		iY = Max((int)ceil(t_y), i_top);
		iYmax = Min((int)t_ymax, i_bottom);

		// Subtract 1 from Y in anticipation of first call to operator++().
		iY--;
	}

	template<class T, class Coord> bool CScanPoly<T, Coord>::operator ++(int) 
	/*
	 *	The iterator.  Increments Y, then calculates iX1 and iX2 for that Y.
	 *	Returns 1 if not done, 0 if done.
	 *
	 * Method: 
	 *	For a given Y, the horizontal line is intersected with every side of the poly.
	 *	The minimum and maximum iX values of the intersections become iX1 and iX2.
	 *
	 ****/
	{
		// Loop until we get to a Y value that isn't clipped away.
		for (;;) 
		{
			iY++;
			if (iY >= iYmax)
				return 0;

			T t_y = (T) iY;

			// Search thru all sides, finding intersections with horizontal line at t_y.
			bool b_found = 0;

			for (int i = 0; i < iCount; i++) {
				int i_2 = i+1;

				if (i_2 == iCount) i_2 = 0;
				if (bBetween(t_y, acoordPoly[i].tY, acoordPoly[i_2].tY))
				{
					// It intersects side i.
					if (acoordPoly[i].tY == acoordPoly[i_2].tY) 
					{
						// The side is a horizontal line, get the 2 points.
						if (acoordPoly[i].tX < acoordPoly[i_2].tX)
						{
							coordLeft = acoordPoly[i];
							coordRight = acoordPoly[i_2];
						}
						else
						{
							coordLeft = acoordPoly[i_2];
							coordRight = acoordPoly[i];
						}
					}
					else 
					{
						//
						// Find the intersecting point.
						// The Coord operators + and * will give us the 
						// value of X at the intersection, as well as all
						// other associated values (e.g. u,v).
						//
						T t = (t_y - acoordPoly[i].tY)
							 / (acoordPoly[i_2].tY - acoordPoly[i].tY);
						Coord coord = acoordPoly[i] * (T(1) - t)
							+ acoordPoly[i_2] * t;

						if (!b_found)
							coordLeft = coordRight = coord;
						else 
						{
							if (coord.tX < coordLeft.tX)
								coordLeft = coord;
							if (coord.tX > coordRight.tX)
								coordRight = coord;
						}
					}
					b_found = 1;
				}
			}

			//
			// Now we have left and right coords.
			// Calculate x increment.
			//
			if (coordLeft.tX != coordRight.tX)
				coordInc = (coordRight - coordLeft) / (coordRight.tX - coordLeft.tX);

			//
			// Now round the coordinates to integers.
			//
			iX1 = (int) Max((T) ceil(coordLeft.tX), Left);
			iX2 = (int) Min((T) floor(coordRight.tX), Right);
			if (iX1 > iX2)
				continue;

			// Adjust associated values.
			coordLeft += coordInc * ((T)iX1 - coordLeft.tX);
			coordRight += coordInc * ((T)iX2 - coordRight.tX);

			if (coordLeft.tZ < (T)0.1 || coordRight.tZ < (T)0.1)
				continue;

			// Line is visible.
			return 1;
		}
	}

