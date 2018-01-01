/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1997
 *
 * Contents:
 *		'Global' static data declarations for the wavelet multiresolution classes.
 *
 * Bugs:
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/GeomDBase/WaveletStaticData.hpp                                   $
 * 
 * 8     3/13/98 4:35p Mlange
 * Removed disable update functionality.
 * 
 * 7     1/21/98 5:28p Mlange
 * Changed interface for allocating the fastheaps for the wavelet quad tree types. Improved some
 * stat printing.
 * 
 * 6     98/01/06 15:41 Speter
 * Moved update flags from CTerrain to NMultiResolution.
 * 
 * 5     12/03/97 8:10p Mlange
 * Added function for allocating and freeing the fastheaps used by the wavelet classes.
 * 
 * 4     10/15/97 6:52p Mlange
 * Removed static CFastHeap used for quad tree type allocations. It is now dynamically allocated
 * when needed instead.
 * 
 * 3     10/14/97 2:26p Mlange
 * Added the PrintProfileStats() function. Added several new profile stats.
 * 
 * 2     7/22/97 1:57p Mlange
 * Added extern declaration.
 * 
 * 1     6/27/97 3:39p Mlange
 * Initial revision.
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_GEOMDBASE_WAVELETSTATICDATA_HPP
#define HEADER_LIB_GEOMDBASE_WAVELETSTATICDATA_HPP


class CConsoleBuffer;
class CProfileStatMain;

namespace NMultiResolution
{
	extern CProfileStatMain psWaveletTIN;
	extern CProfileStatMain psWaveletQuery;

	extern bool bAverageStats;			// 'true' if stat averaging should be done.

	class CQuadRootTIN;
	class CQuadRootQuery;

	//**********************************************************************************************
	//
	void AllocMemory();
	//
	// Allocate the memory for use by the classes in the multiresolution namespace.
	//
	// Notes:
	//		Except for the wavelet quad tree transform classes, call this function BEFORE
	//		constructing ANY of the multiresolution namespace classes.
	//
	//		Note that the memory for the multiresolution namespace classes is allocated dynamically
	//		(instead of statically) to avoid order of initialisation problems.
	//
	//**************************************


	//**********************************************************************************************
	//
	void FreeMemory();
	//
	// Free the memory used by the classes in the multiresolution namespace.
	//
	// Notes:
	//		Call this function AFTER all instances of the multiresolution namespace classes have been
	//		destructed.
	//
	//**************************************


	//**********************************************************************************************
	//
	void PrintProfileStats
	(
		CConsoleBuffer& con,
		const CQuadRootTIN* pqntin_root,
		const CQuadRootQuery* pqnq_root
	);
	//
	// Print all the wavelet quad tree stats and profiles to the given console.
	//
	// Notes:
	//		This function does not clear the console before printing.
	//
	//**************************************
};

#endif
