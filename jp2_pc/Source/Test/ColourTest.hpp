/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		namespace TestMain.
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Test/ColourTest.hpp                                                   $
 * 
 * 2     6/21/96 2:36p Pkeet
 * Finished the initial tests on the clut.
 * 
 * 1     6/18/96 3:07p Pkeet
 * 
 * 2     5/17/96 10:22a Pkeet
 * 
 **********************************************************************************************/

#ifndef HEADER_TEST_COLOURTEST_HPP
#define HEADER_TEST_COLOURTEST_HPP

namespace TestMain
{
	void NewRaster(CRaster&);	// After window created.
	void Update();				// Update between frames.
	void Paint(CRaster&);		// Draw at current state.
};

#endif
