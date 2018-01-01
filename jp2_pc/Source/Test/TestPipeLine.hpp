/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Testbed for the rendering pipeline.
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Test/TestPipeLine.hpp                                                 $
 * 
 * 4     10/24/96 6:35p Pkeet
 * Added the 'WorldDBase.hpp' include.
 * 
 * 3     96/08/15 19:03 Speter
 * Fixed bug in call to TestDrawLine.
 * 
 * 2     96/06/18 10:09a Mlange
 * Added function prototype.
 * 
 * 1     28-05-96 8:30p Mlange
 * Testbed for the rendering pipeline.
 * 
 **********************************************************************************************/

#ifndef HEADER_TEST_TESTPIPELINE_HPP
#define HEADER_TEST_TESTPIPELINE_HPP

#include "Lib/EntityDBase/WorldDBase.hpp"


void TestDrawLine(double d_xa, double d_ya, double d_xb, double d_yb, TPixel pix);

#endif

