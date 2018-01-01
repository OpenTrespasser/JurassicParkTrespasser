/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1998
 *
 * Quality settings data declarations.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/EntityDBase/QualitySettings.hpp                                   $
 * 
 * 5     98.09.24 1:38a Mmouni
 * Added multiplier for terrain mesh pixel error.
 * 
 * 4     98.09.22 12:26a Mmouni
 * Added VM texture size to quality settings.
 * 
 * 3     98.09.08 4:37p Mmouni
 * Added scheduler time slices to quality settings.
 * 
 * 2     98.09.04 4:24p Mmouni
 * Added depth sort distance scale.
 * 
 * 1     98.08.19 5:52p Mmouni
 * Initial version.
 * 
 **********************************************************************************************/


//**********************************************************************************************
//
struct QualityData
//
// Prefix: qd
//
// Structure containing all the data used for adjusting redering settings.
//
//**************************************
{
	int		iSkyDrawMode;				// 0 = solid colour, 1 = gradient, 2 = textured.
	float	fPersectiveError;			// Max. pixel error for using linear primitive.
	int		iMinSubdivison;				// Non-adaptive subdivision length.
	float	fAltPerspectiveError;		// Max. pixel error for using linear primitive (terrain).
	int		iAltMinSubdivision;			// Non-adaptive subdivision length (terrain).
	int		iAdaptiveMinSubdivision;	// Adaptive minimum subdivision length.
	bool	bBilinearFilter;			// Use bi-linear filtering on terrain (if available).
	bool	bDisableLargestMip;			// Disable use of the largest mip-map.

	// Depth sort parameters.
	int		iMaxToDepthsort;			// Maximum number of polygons to depthsort.
	float	fSortDistanceScale;			// Amount to scale depth sort distances by.

	// Culling & detail reduction factors.
	float	fDetailReductionScale;		// Mesh detail reduction multiplier.
	float	fCullDistScale;				// Object culling multiplier.
	float	fCullShadowDistScale;		// Shadow culling multiplier

	// Camera adjustment factors.
	float	fFarClipScale;				// Scale for the far clipping plane.

	// Water adjustment factors.
	float	fWaterResScale;				// Water resolution multiplier.

	// Terrain adjustment factors.
	bool	bShadows;					// Shadows enabled?
	int		iMaxSizeReduction;			// Power of two to reduce the max texture node size by.
	float	fTerrainTexelScale;			// Scale for texture resolution.
	float	fTerrainDistanceScale;		// Scale for shadow, dynamic texture, and no texture distance.
	float	fPixelTolAdj;				// Multiplier for pixel error tolerance for terrain mesh.

	// Image adjustment factors.
	float	fResolutionScale;			// Cache resolution scaling factor.

	// Scheduler time slices.
	uint	uCacheMs;					// Number of milliseconds for image cache updates.
	uint	uTerrainMs;					// Number of milliseconds for terrain updates.

	// Maximum amount of physical memory used for texures (in megabytes).
	int		iMaxPhysicalTextureMemory;
};

// The quality settings data.
extern QualityData qdQualitySettings[];
