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
 * $Log:: /JP2_PC/Source/Test/FastBumpTestShell.hpp                                            $
 * 
 * 1     8/30/96 3:35p Pkeet
 * Initial implementation.
 * 
 **********************************************************************************************/

#ifndef HEADER_TEST_FASTBUMPTESTSHELL_HPP
#define HEADER_TEST_FASTBUMPTESTSHELL_HPP

namespace TestMain
{
	void NewRaster(CRaster&);	// After window created.
	void Update();				// Update between frames.
	void Paint(CRaster&);		// Draw at current state.
};

#endif