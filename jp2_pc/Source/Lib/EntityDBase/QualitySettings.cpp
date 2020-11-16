/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1998
 *
 * Quality settings data.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/EntityDBase/QualitySettings.cpp                                   $
 * 
 * 11    98/10/09 0:31 Speter
 * Made water res vary less by quality.
 * 
 * 10    9/29/98 1:36a Kmckis
 * changed the clipping plane values
 * 
 * 9     9/27/98 4:35p Kmckis
 * Made the sky render in all quality settings
 * 
 * 8     98.09.24 1:38a Mmouni
 * Added multiplier for terrain mesh pixel error.
 * 
 * 7     98.09.22 12:26a Mmouni
 * Added VM texture size to quality settings.
 * 
 * 6     98.09.14 12:55p Mmouni
 * Turned down amount of terrain resolution reduction at lower detail levels since it made no
 * measurable speed difference and looked bad.
 * 
 * 5     98.09.08 4:37p Mmouni
 * Added scheduler time slices to quality settings.
 * 
 * 4     98.09.04 4:25p Mmouni
 * Increased mesh detail reduction multiplier.
 * Added depth sort distance multiplier.
 * 
 * 3     8/26/98 5:50p Mmouni
 * Pulled the clipping plane in more on quality setting 0.
 * 
 * 2     98.08.23 4:02p Mmouni
 * Changed depth sort maximums.
 * 
 * 1     98.08.19 5:52p Mmouni
 * Initial version.
 * 
 **********************************************************************************************/

#include "gblinc/common.hpp"
#include "QualitySettings.hpp"


//**********************************************************************************************
QualityData qdQualitySettings[5] =
{
	// Level 0
	{
		2,		// 0 = solid colour, 1 = gradient, 2 = textured.
		2.0f,	// Max. pixel error for using linear primitive.
		48,		// Non-adaptive subdivision length.
		4.0f,	// Max. pixel error for using linear primitive (terrain).
		64,		// Non-adaptive subdivision length (terrain).
		32,		// Adaptive minimum subdivision length.
		false,	// Use bi-linear filtering on terrain (if available).
		true,	// Disable use of the largest mip-map.

		600,	// Maximum number of polygons to depthsort.
		0.5f,	// Amount to scale depth sort distances by.

		0.80f,	// Mesh detail reduction multiplier.
		0.25f,	// Object culling multiplier.
		0.25f,	// Shadow culling multiplier

		0.065f,	// Scale for the far clipping plane.

		0.40f,	// Water resolution multiplier.

		false,	// Shadows enabled?
		2,		// Power of two to reduce the max texture node size by.
		0.66f,	// Scale for texture resolution.
		0.50f,	// Scale for shadow, dynamic texture, and no texture distance.
		1.0f,	// Multiplier for pixel error tolerance for terrain mesh.

		0.70f,	// Cache resolution scaling factor.

		10,		// Number of milliseconds for image cache updates.
		5,		// Number of milliseconds for terrain updates.
		
		12		// Maximum physical texture memory.
	},

	// Level 1
	{
		2,		// 0 = solid colour, 1 = gradient, 2 = textured.
		1.0f,	// Max. pixel error for using linear primitive.
		32,		// Non-adaptive subdivision length.
		3.0f,	// Max. pixel error for using linear primitive (terrain).
		48,		// Non-adaptive subdivision length (terrain).
		24,		// Adaptive minimum subdivision length.
		false,	// Use bi-linear filtering on terrain (if available).
		true,	// Disable use of the largest mip-map.

		700,	// Maximum number of polygons to depthsort.
		0.75f,	// Amount to scale depth sort distances by.

		0.90f,	// Mesh detail reduction multiplier.
		0.50f,	// Object culling multiplier.
		0.50f,	// Shadow culling multiplier

		0.11f,	// Scale for the far clipping plane.

		0.60f,	// Water resolution multiplier.

		false,	// Shadows enabled?
		1,		// Power of two to reduce the max texture node size by.
		0.83f,	// Scale for texture resolution.
		0.75f,	// Scale for shadow, dynamic texture, and no texture distance.
		1.0f,	// Multiplier for pixel error tolerance for terrain mesh.

		0.85f,	// Cache resolution scaling factor.

		10,		// Number of milliseconds for image cache updates.
		5,		// Number of milliseconds for terrain updates.
		
		12		// Maximum physical texture memory.
	},

	// Level 2
	{
		2,		// 0 = solid colour, 1 = gradient, 2 = textured.
		2.0f,	// Max. pixel error for using linear primitive.
		32,		// Non-adaptive subdivision length.
		2.0f,	// Max. pixel error for using linear primitive (terrain).
		48,		// Non-adaptive subdivision length (terrain).
		16,		// Adaptive minimum subdivision length.
		false,	// Use bi-linear filtering on terrain (if available).
		false,	// Disable use of the largest mip-map.

		800,	// Maximum number of polygons to depthsort.
		1.0f,	// Amount to scale depth sort distances by.

		1.00f,	// Mesh detail reduction multiplier.
		0.75f,	// Object culling multiplier.
		0.75f,	// Shadow culling multiplier

		0.33f,	// Scale for the far clipping plane.

		0.80f,	// Water resolution multiplier.

		true,	// Shadows enabled?
		1,		// Power of two to reduce the max texture node size by.
		0.83f,	// Scale for texture resolution.
		0.75f,	// Scale for shadow, dynamic texture, and no texture distance.
		1.0f,	// Multiplier for pixel error tolerance for terrain mesh.

		1.0f,	// Cache resolution scaling factor.

		10,		// Number of milliseconds for image cache updates.
		5,		// Number of milliseconds for terrain updates.
		
		16		// Maximum physical texture memory.
	},

	// Level 3
	{
		2,		// 0 = solid colour, 1 = gradient, 2 = textured.
		1.0f,	// Max. pixel error for using linear primitive.
		32,		// Non-adaptive subdivision length.
		1.0f,	// Max. pixel error for using linear primitive (terrain).
		32,		// Non-adaptive subdivision length (terrain).
		16,		// Adaptive minimum subdivision length.
		true,	// Use bi-linear filtering on terrain (if available).
		false,	// Disable use of the largest mip-map.

		900,	// Maximum number of polygons to depthsort.
		1.0f,	// Amount to scale depth sort distances by.

		1.0f,	// Mesh detail reduction multiplier.
		1.0f,	// Object culling multiplier.
		1.0f,	// Shadow culling multiplier

		0.6f,	// Scale for the far clipping plane.

		1.0f,	// Water resolution multiplier.

		true,	// Shadows enabled?
		0,		// Power of two to reduce the max texture node size by.
		1.0f,	// Scale for texture resolution.
		1.0f,	// Scale for shadow, dynamic texture, and no texture distance.
		1.0f,	// Multiplier for pixel error tolerance for terrain mesh.

		1.2f,	// Cache resolution scaling factor.

		10,		// Number of milliseconds for image cache updates.
		5,		// Number of milliseconds for terrain updates.

		16		// Maximum physical texture memory.
	},

	// Level 4
	{
		2,		// 0 = solid colour, 1 = gradient, 2 = textured.
		1.0f,	// Max. pixel error for using linear primitive.
		16,		// Non-adaptive subdivision length.
		1.0f,	// Max. pixel error for using linear primitive (terrain).
		32,		// Non-adaptive subdivision length (terrain).
		8,		// Adaptive minimum subdivision length.
		true,	// Use bi-linear filtering on terrain (if available).
		false,	// Disable use of the largest mip-map.

		1000,	// Maximum number of polygons to depthsort.
		1.25f,	// Amount to scale depth sort distances by.

		4.0f,	// Mesh detail reduction multiplier.
		1.25f,	// Object culling multiplier.
		1.25f,	// Shadow culling multiplier

		1.0f,	// Scale for the far clipping plane.

		1.20f,	// Water resolution multiplier.

		true,	// Shadows enabled?
		0,		// Power of two to reduce the max texture node size by.
		1.5f,	// Scale for texture resolution.
		1.5f,	// Scale for shadow, dynamic texture, and no texture distance.
		0.5f,	// Multiplier for pixel error tolerance for terrain mesh.

		1.2f,	// Cache resolution scaling factor.

		14,		// Number of milliseconds for image cache updates.
		7,		// Number of milliseconds for terrain updates.

		16		// Maximum physical texture memory.
	}
};
