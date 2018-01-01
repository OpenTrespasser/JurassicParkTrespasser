/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Contents:
 *		Implementation of 'WaterTest.hpp.'
 *
 * To do:
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/GUIApp/WaterTest.cpp                                                  $
 * 
 * 12    3/18/98 4:08p Pkeet
 * Added the 'PartitionPriv.hpp' include.
 * 
 * 11    1/22/98 2:47p Pkeet
 * Added the camera include to accomodate the reduction in the number of includes in 'StdAfx.h.'
 * 
 * 10    10/29/97 7:38p Gfavor
 * Changed default size.
 * 
 * 9     97/10/02 12:37 Speter
 * Now always add water at camera position.  Changed default size to be very wide water, for
 * testing selective integration.
 * 
 * 8     97/07/09 16:36 Speter
 * Changed CMeshRect to CMeshPolygon.  
 * 
 * 7     97/02/16 16:58 Speter
 * Removed unneeded includes.  Removed code that sets render flags, as they're now set by
 * default.
 * 
 * 6     97/02/16 16:36 Speter
 * Now call CEntityWater constructor with SInit object.
 * 
 * 5     97/02/15 20:57 Speter
 * Updated for new CSimWater2D constructor.  Removed commented code which adds initial bump.
 * 
 * 4     97/02/14 13:48 Speter
 * Use new default parameters for water damping and restoring.
 * 
 * 3     97/01/31 19:13 Speter
 * Added constants.  Now place water at suitable position for terrain interaction.
 * 
 * 2     97/01/16 11:36 Speter
 * Updated for change in CScreenRender.
 * 
 * 1     96/12/31 17:31 Speter
 * Moved from Test/WaterTest directory.
 * 
 * 12    96/12/06 15:21 Speter
 * When water is added, it now forces certain render states.
 * 
 * 11    96/12/05 14:34 Speter
 * Tweaked params some more.
 * 
 * 10    96/12/04 20:29 Speter
 * Construct CPresence3<> for Instance constructor.
 * Changed water test size.
 * 
 * 9     96/11/21 18:58 Speter
 * Changed parameters for constructor.
 * 
 * 8     11/21/96 11:54a Mlange
 * Updated for changes to CPresence.
 * 
 * 7     96/11/20 20:04 Speter
 * Set damping back to a more reasonable value.
 * 
 * 6     96/11/20 17:09 Speter
 * Changed dimensions of water object from 100 to 10 meters.  Changed scale of water entity to
 * 1, now that camera clipping planes are realistic.
 * 
 * 5     96/11/14 17:54 Speter
 * Updated for change in CEntityWater, which now makes its own shape.
 * 
 * 4     96/11/11 17:06 Speter
 * Removed ExecuteWater() function, replaced with simple WaterLoad(), which adds a CEntityWater.
 * 
 * 2     11/05/96 7:05p Pkeet
 * Added capability to create a bumpmap without texture and heightfield files.
 * 
 * 1     11/04/96 6:59p Pkeet
 * Initial implementation.
 * 
 **********************************************************************************************/


//
// Includes.
//
#include "stdafx.h"
#include "Lib/GeomDBase/PartitionPriv.hpp"
#include "WaterTest.hpp"

//
// Warning!  If Water.hpp appears before Waves.hpp or Mesh.hpp, the linker pukes with an
// "unknown error".  My guess is that this happens because opaque classes in CEntityWater
// are declared before actual definitions of those classes. --speter
//
#include "Lib/GeomDBase/Mesh.hpp"
#include "Lib/EntityDBase/Water.hpp"
#include "Lib/EntityDBase/WorldDBase.hpp"
#include "Lib/EntityDBase/Query/QRenderer.hpp"
#include "Lib/Renderer/Camera.hpp"

const TReal		rWATER_LENGTH	= 20;
const TReal		rWATER_WIDTH	= 10;
const TReal		rWATER_HEIGHT	= 1.25;

//*********************************************************************************************
void WaterLoad()
{
	//
	// Add pool to world database, at current camera position.
	//

	CPresence3<> pr3_water = CWDbQueryActiveCamera().tGet()->pr3Presence();
	pr3_water.v3Pos.tZ -= rWATER_HEIGHT;

	CEntityWater* petwater = new CEntityWater(CInstance::SInit
	(
		// Initial position of water.
		pr3_water,

		// Rectangular mesh specifying water dimensions.
		rptr_cast(CRenderType, rptr_new CMeshPolygon(rWATER_LENGTH, rWATER_WIDTH))
	));

	wWorld.Add(petwater);
}

