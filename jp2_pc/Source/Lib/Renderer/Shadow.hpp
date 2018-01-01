/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		CShadowBuffer
 *
 * Bugs:
 *
 * To do:
 *		Change to an 8-bit pixel value when supported by the rasteriser.
 *		Add multi-resolution sub-buffers, with management.
 *		If ever necessary, add buffer for points lights, with multiple surfaces and perspective 
 *		projection.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/Renderer/Shadow.hpp                                               $
 * 
 * 12    4/13/98 4:44p Mlange
 * Rendering of moving and/or static shadows is now optional.
 * 
 * 11    97/08/19 22:02 Speter
 * Removed r_zmin param from ShadowLighting(), as it can just use 0.
 * 
 * 10    97/08/18 10:38 Speter
 * Simplified shadowing interface, removing unnecessary calculations.
 * 
 * 9     97/08/17 19:24 Speter
 * Now creates proper directional shadows.
 * 
 * 8     97/08/15 12:10 Speter
 * Added ShadowLighting function, for shadowing a composite texture/light map.
 * 
 * 7     97/07/07 14:15 Speter
 * CShadowBuffer::Render() now takes a top-level partition rather than a partition list.
 * 
 * 6     97-05-07 20:45 Speter
 * Changed shadow pixel from uint16 to uint8.
 * 
 * 5     97-05-06 16:08 Speter
 * Updated, simplified, for new parallel-capable CCamera.
 * 
 * 4     4/30/97 9:19p Rwyatt
 * Moved esfView member variable from CPartition and put it into the list that the partition
 * functions return. These functions used to return a list of CPartition* now they return a list
 * of structures that contain a CPartition* and esfSideOf.
 * 
 * 3     97-03-28 16:20 Speter
 * First somewhat working shadow implementation.
 * Shadowing primitive now works; uses new CScreenRenderShadow rasteriser, and a 16-bit Z
 * buffer.
 * 
 * 2     97/03/24 15:11 Speter
 * Further progress, still not functioning.
 * 
 * 1     97/03/20 17:03 Speter
 * 
 **********************************************************************************************/

#ifndef HEADER_LIB_RENDERER_SHADOW_HPP
#define HEADER_LIB_RENDERER_SHADOW_HPP

#include "Lib/Transform/Transform.hpp"
#include "Lib/Transform/TransLinear.hpp"
#include "Lib/View/Raster.hpp"
#include "Lib/EntityDBase/Instance.hpp"
#include "Lib/Renderer/GeomTypes.hpp"

#include <list.h>

//
// Definitions for CShadowBuffer.
//

//**********************************************************************************************
typedef uint8	TShadowPixel;
//
// Prefix: shpix
//
// Type representing shadow height information in the shadow buffer.
//
//**************************************

// The largest value storable in the shadow buffer.
const TShadowPixel		shpixMAX			= UTypeMax(TShadowPixel);

// Create the Z-buffer scaling values to map the range [0..1] to [0..shpixMAX].
const float fSHADOW_Z_MULTIPLIER			= shpixMAX;

//**********************************************************************************************
//
class CShadowBuffer: public CRasterMemT<TShadowPixel>
//
// Prefix: shbuf
//
// A raster that contains shadow height information for a directional light.  The raster
// corresponds to a plane in the world perpendicular to the light direction.  Currently
// only works for directional lights.
//
//**************************************
{
private:
	CCamera*			pCamera;			// The camera used to render this shadow buffer.
	CTransform3<>		tf3ObjectCamera;	// Transform to map object points to the buffer.

public:

	//******************************************************************************************
	//
	// Constructor.
	//

	//******************************************************************************************
	CShadowBuffer
	(
		int i_dim,						// The number of pixels in each dimension for the buffer.
		const CBoundVol& bv_world,		// The extents of the world volume to shadow.
		const CPlacement3<>& p3_light	// Placement of the shadowing light.
	);
	//
	// Creates a shadow buffer of the given size, representing the given square in the world.
	// Note that this square exists at world Z = 0. The volume handled by this buffer is this
	// square extruded in the direction of the light, *not* straight up.
	//

	//******************************************************************************************
	//
	// Member functions
	//

	//******************************************************************************************
	void Render
	(
		CPartition* ppart			// The partition for which to render shadows.
	);
	//
	// Render the shapes' height information to the buffer.
	//

	//******************************************************************************************
	//
	void SetViewingContext
	(
		const CPresence3<>& pr3		// Coordinate space in which to perform calculations.
									// (typically a world-to-object transformation).
	);
	//
	// Set any internal parameters associated with the viewing context.
	//
	//**********************************

	//******************************************************************************************
	bool bWithinBuffer
	(
		const CVector3<>& v3_object	// Location in object space (as defined by SetViewingContext).
	);
	//
	// Returns:
	//		Whether the given point is in the extent of the shadow buffer.
	//

	//******************************************************************************************
	bool bShadowed
	(
		const CVector3<>& v3_object	// Location in object space (as defined by SetViewingContext).
	) const;
	//
	// Returns:
	//		Whether the given point is in shadow for this light.
	//
	// Notes:
	//		The point must be within the shadow buffer extent (checked by Assert).
	//
};

//******************************************************************************************
//
void ShadowLighting
(
	rptr<CRaster> pras_composite,			// Composite raster to fill.
	const CRectangle<>& rc_world,			// Section of world to shadow.
	const CDir3<>& d3_light,				// Direction of the shadowing light.
	int i_shadow_level,						// Light value to set in the map.
	bool b_moving_shadows,					// Whether to render shadows cast by moving objs.
	bool b_static_shadows					// Whether to render shadows cast by static objs.
);
//
// Renders shadows into a composite texel/lexel map.
// Totally different from above shadowing system.
//
//**************************************

#endif
