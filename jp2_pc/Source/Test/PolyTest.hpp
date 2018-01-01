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
 * $Log:: /JP2_PC/Source/Test/PolyTest.hpp                                                     $
 * 
 * 2     5/17/96 10:22a Pkeet
 * 
 **********************************************************************************************/

#ifndef HEADER_TEST_POLYTEST_HPP
#define HEADER_TEST_POLYTEST_HPP

namespace TestMain
{
	void NewRaster(CRaster&);	// After window created.
	void Update();			// Update between frames.
	void Paint(CRaster&);		// Draw at current state.
};

#endif
