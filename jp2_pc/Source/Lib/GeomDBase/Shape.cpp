/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1996
 *
 * Implementation of Shape.hpp.
 *
 * To do:
 *		Perhaps move pshGetProperShape to pipeline, where it can more easily access the settings.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/GeomDBase/Shape.cpp                                               $
 * 
 * 38    98.08.19 6:01p Mmouni
 * Moved all quality setting values and multipliers into a single structure.
 * 
 * 37    8/11/98 2:16p Agrant
 * changed include to allow compilation
 * 
 * 36    98/06/29 16:06 Speter
 * Made iNumTriangles() virtual.
 * 
 * 35    98.06.25 8:45p Mmouni
 * Added stuff to support render quality setting.
 * 
 * 34    98/03/17 12:02 Speter
 * More robust handling of circular reference.
 * 
 * 33    3/12/98 11:23a Pkeet
 * Fixed the circular rptr reference bug that was preventing meshes from being deleted on exit,
 * and was therefore the source of numerous memory leaks.
 * 
 * 32    3/03/98 4:56p Pkeet
 * Put in a fix for models that are not loaded through .grf files.
 * 
 * 31    3/03/98 3:27p Pkeet
 * Added the 'pshShadow' data member and the 'pshGetTerrainShape' and 'SetShadowShape' member
 * functions.
 * 
 * 30    98/02/26 14:09 Speter
 * Replaced CPolyIterator::sfSurface() with ptexTexture(). Moved seterfFeatures to CTexture from
 * SSurface, removing redundant flags. Removed SSurface as global struct, replaced with straight
 * rptr<CTexture>.
 * 
 * 29    2/10/98 1:41p Pkeet
 * Added code to remove texturing from surfaces.
 * 
 * 28    2/02/98 2:52p Mlange
 * Added CShape::rPolyPlaneThickness().
 * 
 * 27    1/18/98 1:02p Pkeet
 * Added a 'Validate' member function.
 * 
 * 26    1/16/98 6:33p Pkeet
 * Activates the copy flag for 16 bit bitmaps that are not bump maps.
 * 
 * 25    1/15/98 10:14p Agrant
 * Use the mesh-dependent detail reduction settings
 * 
 * 24    97/10/15 4:31p Pkeet
 * Changed the render member function to accept a render cache pointer as a parameter. Added the
 * 'bCache' flag. Added the 'bCache' member function to the polygon iterator class. Added the
 * 'SetPolygonAsCache' member function.
 * 
 * 23    97/10/13 2:15p Pkeet
 * Fixed bug with alpha.
 * 
 * 22    97/10/12 22:09 Speter
 * Removed pmatMaterial from SSurface (now stored in clut).  Moved seterfDEFAULT def to .cpp.
 * Constructor which takes render flags now optionally calls UpdateFeatures().  Updated feature
 * setting.
 * 
 * 21    97/10/01 7:32p Pkeet
 * Added code so the SSurface constructor sets its erf flag for occlusion if the texture for the
 * surface is an occlusion polygon signal.
 * 
 * 20    97/08/14 6:41p Pkeet
 * Added support for alpha textures in the SSurface struct.
 * 
 **********************************************************************************************/

#include "common.hpp"
#include "Shape.hpp"
#include "Lib/View/Raster.hpp"
#include "Lib/Renderer/PipeLine.hpp"
#include "Lib/EntityDBase/WorldDBase.hpp"
#include "Lib/EntityDBase/QualitySettings.hpp"


//**********************************************************************************************
//
// CShape implementation.
//

	TReal CShape::rPolyPlaneThicknessDefault = 0.0002;
	TReal CShape::rPolyPlaneThicknessD3D     = -0.05;


	const TReal rLEVEL_MULTIPLE	= 0.25;			// Ratio between preferred areas for each level.

	//******************************************************************************************
	rptr_const<CShape> CShape::pshGetProperShape(TReal r_screen_area) const
	{
		AlwaysAssert(this);
		rptr_const<CShape> psh = rptr_const_this(this);
		AlwaysAssert(psh);

		// Apply global quality setting multiplier.
		r_screen_area *= prenMain->pSettings->fDetailReduceFactor * 
						 qdQualitySettings[iGetQualitySetting()].fDetailReductionScale;

		// Check area against each level's customised value.
		// If the screen area is less than our minimum, use the lower detail level.
		while (psh->pshCoarser && r_screen_area < psh->rMinScreenArea)
			psh = psh->pshCoarser;

		return psh;
	}

	//******************************************************************************************
	rptr_const<CShape> CShape::pshGetTerrainShape() const
	{
		// For terrain, just use the terrain shadow object.
		if (pshShadow)
		{
			// If there is a special shadow mesh, return it.
			return pshShadow;
		}
		else
		{
			// If there is no special shadow mesh, just return this one.
			return rptr_const_this(this);
		}
	}

	//******************************************************************************************
	void CShape::SetShadowShape(rptr<CShape>& rpsh)
	{
		//
		// Use a pre-defined shadow shape. If no pre-defined shape exists, use the lowest detail
		// level shape available.
		//
		pshShadow = rpsh;
		if (!pshShadow)
		{
			if (pshCoarser)
			{
				pshShadow = pshCoarser;
				while (pshShadow->pshCoarser)
					pshShadow = pshShadow->pshCoarser;
			}
			else
			{
				pshShadow = rptr0;
			}
		}
		// Handle circular reference to this mesh specially.
		else if (pshShadow == rptr_this(this))
			pshShadow = rptr0;
	}

	//******************************************************************************************
	TReal CShape::rPolyPlaneThickness() const
	{
		return CShape::rPolyPlaneThicknessDefault;
	}


	//******************************************************************************************
	CPArray< CVector3<> > CShape::pav3GetWrap() const
	{
		Assert(0);
		return 0;
	}
	
	//******************************************************************************************
	void CShape::CreateWrap()
	{
		Assert(0);
	}
	
	//******************************************************************************************
	void CShape::CreateWrapBox()
	{
		Assert(0);
	}

	//******************************************************************************************
	void CShape::CreateMipMaps()
	{
		Assert(0);
	}
	
	//******************************************************************************************
	void CShape::Validate() const
	{
	}


