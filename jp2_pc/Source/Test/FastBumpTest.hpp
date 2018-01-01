/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Module to test the operation of the clut.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Test/FastBumpTest.hpp                                                 $
 * 
 * 3     9/05/96 2:28p Pkeet
 * Added texturing to bumpmapping.
 * 
 * 2     9/04/96 5:17p Pkeet
 * Added file-based bumpmaps.
 * 
 * 1     8/30/96 3:35p Pkeet
 * Initial implementation.
 * 
 **********************************************************************************************/

#ifndef HEADER_TEST_FASTBUMPTEST_HPP
#define HEADER_TEST_FASTBUMPTEST_HPP

//
// Includes.
//

#include "Lib/View/Clut.hpp"
#include "Lib/Renderer/Primitives/FastBump.hpp"


const int iBumpmapLen = 128;


//
// Global functions.
//

//*********************************************************************************************
//
void DrawSpeedBump
//
// Draws a bumpmap onto a raster.
//
//**************************************
(
	CRaster* prasDest,
	int      i_xshift = 0,
	int      i_yshift = 0
);

void MakeBumpmap();

extern CBumpAnglePair Light;


#endif
