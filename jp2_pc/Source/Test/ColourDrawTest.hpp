/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Module to test the operation of the clut.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Test/ColourDrawTest.hpp                                               $
 * 
 * 8     7/18/96 3:35p Pkeet
 * Changed to use 'CRender' style.
 * 
 * 7     7/13/96 10:59p Pkeet
 * 
 * 6     7/13/96 10:58p Pkeet
 * Finished test.
 * 
 * 5     7/13/96 5:51p Pkeet
 * Speedbump test.
 * 
 * 4     6/21/96 2:36p Pkeet
 * Finished the initial tests on the clut.
 * 
 * 3     6/19/96 8:37p Pkeet
 * Initial implementation.
 * 
 * 2     6/18/96 4:26p Pkeet
 * 
 * 1     6/18/96 4:23p Pkeet
 * 
 **********************************************************************************************/

#ifndef HEADER_TEST_COLOURDRAWTEST_HPP
#define HEADER_TEST_COLOURDRAWTEST_HPP

//
// Includes.
//

#include "Lib/View/Clut.hpp"


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

//*********************************************************************************************
//
void DrawWater
//
// Draws a watermap onto a raster.
//
//**************************************
(
	CRaster* prasDest
);


enum EBumpmap { cone, hemi, ripple };

extern void MakeBumpmap(EBumpmap ebm);

extern int iIncidence;
extern int iBearing;
extern int iConeHeight;


#endif
